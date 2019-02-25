// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Vertexing/LinearizedTrackFactory.hpp"
#include "Acts/Vertexing/TrackAtVertex.hpp"

namespace {

/// @struct BilloirTrack
///
/// @brief Struct to cache track-specific matrix operations in Billoir fitter
template <typename InputTrack>
struct BilloirTrack
{
  BilloirTrack(const InputTrack& params, Acts::LinearizedTrack* lTrack)
    : originalTrack(params), linTrack(lTrack)
  {
  }

  BilloirTrack(const BilloirTrack& arg) = default;

  const InputTrack       originalTrack;
  Acts::LinearizedTrack* linTrack;
  double                 chi2;
  Acts::ActsMatrixD<5, 3> DiMat;   // position jacobian
  Acts::ActsMatrixD<5, 3> EiMat;   // momentum jacobian
  Acts::ActsSymMatrixD<3> GiMat;   // Gi = EtWmat * Emat (see below)
  Acts::ActsSymMatrixD<3> BiMat;   // Bi = Di.T * Wi * Ei
  Acts::ActsSymMatrixD<3> CiInv;   // Ci = (Ei.T * Wi * Ei)^-1
  Acts::Vector3D          UiVec;   // Ui = Ei.T * Wi * dqi
  Acts::ActsSymMatrixD<3> BCiMat;  // BCi = Bi * Ci^-1
  Acts::ActsVectorD<5>    deltaQ;
};

/// @struct BilloirVertex
///
/// @brief Struct to cache vertex-specific matrix operations in Billoir fitter
struct BilloirVertex
{
  BilloirVertex() = default;

  Acts::ActsSymMatrixD<3> Amat{
      Acts::ActsSymMatrixD<3>::Zero()};         // T  = sum{Di.T * Wi * Di}
  Acts::Vector3D Tvec{Acts::Vector3D::Zero()};  // A  = sum{Di.T * Wi * dqi}
  Acts::ActsSymMatrixD<3> BCBmat{
      Acts::ActsSymMatrixD<3>::Zero()};  // BCB = sum{Bi * Ci^-1 * Bi.T}
  Acts::Vector3D BCUvec{Acts::Vector3D::Zero()};  // BCU = sum{Bi * Ci^-1 * Ui}
};

}  // end anonymous namespace

template <typename BField, typename InputTrack, typename Propagator_t>
Acts::Vertex<InputTrack>
Acts::FullBilloirVertexFitter<BField, InputTrack, Propagator_t>::fit(
    const std::vector<InputTrack>& paramVector,
    const Propagator_t&            propagator,
    Vertex<InputTrack>             constraint) const
{
  double       chi2    = std::numeric_limits<double>::max();
  double       newChi2 = 0;
  unsigned int nTracks = paramVector.size();

  if (nTracks == 0) {
    return Vertex<InputTrack>(Vector3D(0., 0., 0.));
  }

  // Set number of degrees of freedom
  int ndf = 2 * nTracks - 3;
  if (nTracks < 2) {
    ndf = 1;
  }

  // Determine if we do contraint fit or not
  bool isConstraintFit = false;
  if (constraint.covariance().trace() != 0) {
    isConstraintFit = true;
    ndf += 3;
  }

  // Factory for linearizing tracks
  typename LinearizedTrackFactory<BField, Propagator_t>::Config lt_config(
      m_cfg.bField);
  LinearizedTrackFactory<BField, Propagator_t> linFactory(lt_config);

  std::vector<BilloirTrack<InputTrack>> billoirTracks;

  std::vector<Vector3D> trackMomenta;

  Vector3D linPoint(constraint.position());

  Vertex<InputTrack> fittedVertex;

  for (int nIter = 0; nIter < m_cfg.maxIterations; ++nIter) {
    billoirTracks.clear();

    newChi2 = 0;

    BilloirVertex billoirVertex;
    int           iTrack = 0;
    // iterate over all tracks
    for (const InputTrack& trackContainer : paramVector) {
      const auto& trackParams = extractParameters(trackContainer);
      if (nIter == 0) {
        double phi   = trackParams.parameters()[ParID_t::ePHI];
        double theta = trackParams.parameters()[ParID_t::eTHETA];
        double qop   = trackParams.parameters()[ParID_t::eQOP];
        trackMomenta.push_back(Vector3D(phi, theta, qop));
      }
      LinearizedTrack linTrack
          = linFactory.linearizeTrack(&trackParams, linPoint, propagator);
      double d0     = linTrack.parametersAtPCA[ParID_t::eLOC_D0];
      double z0     = linTrack.parametersAtPCA[ParID_t::eLOC_Z0];
      double phi    = linTrack.parametersAtPCA[ParID_t::ePHI];
      double theta  = linTrack.parametersAtPCA[ParID_t::eTHETA];
      double qOverP = linTrack.parametersAtPCA[ParID_t::eQOP];

      // calculate f(V_0,p_0)  f_d0 = f_z0 = 0
      double                   fPhi   = trackMomenta[iTrack][0];
      double                   fTheta = trackMomenta[iTrack][1];
      double                   fQOvP  = trackMomenta[iTrack][2];
      BilloirTrack<InputTrack> currentBilloirTrack(trackContainer, &linTrack);

      // calculate deltaQ[i]
      currentBilloirTrack.deltaQ[0] = d0;
      currentBilloirTrack.deltaQ[1] = z0;
      currentBilloirTrack.deltaQ[2] = phi - fPhi;
      currentBilloirTrack.deltaQ[3] = theta - fTheta;
      currentBilloirTrack.deltaQ[4] = qOverP - fQOvP;

      // position jacobian (D matrix)
      ActsMatrixD<5, 3> Dmat;
      Dmat = linTrack.positionJacobian;

      // momentum jacobian (E matrix)
      ActsMatrixD<5, 3> Emat;
      Emat = linTrack.momentumJacobian;
      // cache some matrix multiplications
      ActsMatrixD<3, 5> DtWmat;
      DtWmat.setZero();
      ActsMatrixD<3, 5> EtWmat;
      EtWmat.setZero();
      DtWmat = Dmat.transpose() * (linTrack.covarianceAtPCA.inverse());
      EtWmat = Emat.transpose() * (linTrack.covarianceAtPCA.inverse());

      // compute billoir tracks
      currentBilloirTrack.DiMat = Dmat;
      currentBilloirTrack.EiMat = Emat;
      currentBilloirTrack.GiMat = EtWmat * Emat;
      currentBilloirTrack.BiMat = DtWmat * Emat;  // Di.T * Wi * Ei
      currentBilloirTrack.UiVec
          = EtWmat * currentBilloirTrack.deltaQ;  // Ei.T * Wi * dqi
      currentBilloirTrack.CiInv
          = (EtWmat * Emat).inverse();  // (Ei.T * Wi * Ei)^-1

      // sum up over all tracks
      billoirVertex.Tvec
          += DtWmat * currentBilloirTrack.deltaQ;  // sum{Di.T * Wi * dqi}
      billoirVertex.Amat += DtWmat * Dmat;         // sum{Di.T * Wi * Di}

      // remember those results for all tracks
      currentBilloirTrack.BCiMat = currentBilloirTrack.BiMat
          * currentBilloirTrack.CiInv;  // BCi = Bi * Ci^-1

      // and some summed results
      billoirVertex.BCUvec += currentBilloirTrack.BCiMat
          * currentBilloirTrack.UiVec;  // sum{Bi * Ci^-1 * Ui}
      billoirVertex.BCBmat += currentBilloirTrack.BCiMat
          * currentBilloirTrack.BiMat.transpose();  // sum{Bi * Ci^-1 * Bi.T}

      billoirTracks.push_back(currentBilloirTrack);
      ++iTrack;

    }  // end loop tracks

    // calculate delta (billoirFrameOrigin-position), might be changed by the
    // beam-const
    Vector3D Vdel = billoirVertex.Tvec
        - billoirVertex.BCUvec;  // Vdel = T-sum{Bi*Ci^-1*Ui}
    ActsSymMatrixD<3> VwgtMat = billoirVertex.Amat
        - billoirVertex.BCBmat;  // VwgtMat = A-sum{Bi*Ci^-1*Bi.T}

    if (isConstraintFit) {

      Vector3D isConstraintFitPosInBilloirFrame;
      isConstraintFitPosInBilloirFrame.setZero();
      // this will be 0 for first iteration but != 0 from second on
      isConstraintFitPosInBilloirFrame[0]
          = constraint.position()[0] - linPoint[0];
      isConstraintFitPosInBilloirFrame[1]
          = constraint.position()[1] - linPoint[1];
      isConstraintFitPosInBilloirFrame[2]
          = constraint.position()[2] - linPoint[2];

      Vdel += constraint.covariance().inverse()
          * isConstraintFitPosInBilloirFrame;
      VwgtMat += constraint.covariance().inverse();
    }

    // cov(deltaV) = VwgtMat^-1
    ActsSymMatrixD<3> covDeltaVmat = VwgtMat.inverse();

    // deltaV = cov_(deltaV) * Vdel;
    Vector3D deltaV = covDeltaVmat * Vdel;

    //--------------------------------------------------------------------------------------
    // start momentum related calculations

    std::vector<std::unique_ptr<ActsSymMatrixD<5>>> covDeltaPmat(nTracks);

    iTrack = 0;
    for (auto& bTrack : billoirTracks) {

      Vector3D deltaP
          = (bTrack.CiInv) * (bTrack.UiVec - bTrack.BiMat.transpose() * deltaV);

      // update track momenta
      trackMomenta[iTrack][0] += deltaP[0];
      trackMomenta[iTrack][1] += deltaP[1];
      trackMomenta[iTrack][2] += deltaP[2];

      // correct for 2PI / PI periodicity
      double tmpPhi = std::fmod(trackMomenta[iTrack][0], 2 * M_PI);  // temp phi
      if (tmpPhi > M_PI) {
        tmpPhi -= 2 * M_PI;
      }
      if (tmpPhi < -M_PI && tmpPhi > -2 * M_PI) {
        tmpPhi += 2 * M_PI;
      }

      double tmpTht
          = std::fmod(trackMomenta[iTrack][1], 2 * M_PI);  // temp theta
      if (tmpTht < -M_PI) {
        tmpTht = std::abs(tmpTht + 2 * M_PI);
      } else if (tmpTht < 0) {
        tmpTht *= -1;
        tmpPhi += M_PI;
        tmpPhi = tmpPhi > M_PI ? tmpPhi - 2 * M_PI : tmpPhi;
      }
      if (tmpTht > M_PI) {
        tmpTht = 2 * M_PI - tmpTht;
        tmpPhi += M_PI;
        tmpPhi = tmpPhi > M_PI ? (tmpPhi - 2 * M_PI) : tmpPhi;
      }

      trackMomenta[iTrack][0] = tmpPhi;
      trackMomenta[iTrack][1] = tmpTht;

      // calculate 5x5 covdelta_P matrix
      // d(d0,z0,phi,theta,qOverP)/d(x,y,z,phi,theta,qOverP)-transformation
      // matrix
      ActsMatrixD<5, 6> transMat;
      transMat.setZero();
      transMat(0, 0) = bTrack.DiMat(0, 0);
      transMat(0, 1) = bTrack.DiMat(0, 1);
      transMat(1, 0) = bTrack.DiMat(1, 0);
      transMat(1, 1) = bTrack.DiMat(1, 1);
      transMat(1, 2) = 1.;
      transMat(2, 3) = 1.;
      transMat(3, 4) = 1.;
      transMat(4, 5) = 1.;

      // some intermediate calculations to get 5x5 matrix
      // cov(V,V)
      ActsSymMatrixD<3> VVmat;
      VVmat.setZero();
      VVmat = covDeltaVmat;

      // cov(V,P)
      ActsSymMatrixD<3> VPmat;
      VPmat.setZero();
      VPmat = -covDeltaVmat * bTrack.GiMat * bTrack.CiInv;

      // cov(P,P)
      ActsSymMatrixD<3> PPmat;
      PPmat.setZero();
      PPmat = bTrack.CiInv
          + bTrack.BCiMat.transpose() * covDeltaVmat * bTrack.BCiMat;

      ActsSymMatrixD<6> covMat;
      covMat.setZero();
      covMat.block<3, 3>(0, 3) = VPmat;
      covMat.block<3, 3>(3, 0) = VPmat.transpose();
      covMat.block<3, 3>(0, 0) = VVmat;
      covMat.block<3, 3>(3, 3) = PPmat;

      // covdelta_P calculation
      covDeltaPmat[iTrack] = std::make_unique<ActsSymMatrixD<5>>(
          transMat * covMat * transMat.transpose());
      // Calculate chi2 per track.
      bTrack.chi2
          = ((bTrack.deltaQ - bTrack.DiMat * deltaV - bTrack.EiMat * deltaP)
                 .transpose()
             * bTrack.linTrack->covarianceAtPCA.inverse()
             * (bTrack.deltaQ - bTrack.DiMat * deltaV
                - bTrack.EiMat * deltaP))[0];
      newChi2 += bTrack.chi2;

      ++iTrack;
    }

    if (isConstraintFit) {
      Vector3D deltaTrk;
      deltaTrk.setZero();
      // last term will also be 0 again but only in the first iteration
      // = calc. vtx in billoir frame - (    isConstraintFit pos. in billoir
      // frame )
      deltaTrk[0] = deltaV[0] - (constraint.position()[0] - linPoint[0]);
      deltaTrk[1] = deltaV[1] - (constraint.position()[1] - linPoint[1]);
      deltaTrk[2] = deltaV[2] - (constraint.position()[2] - linPoint[2]);
      newChi2 += (deltaTrk.transpose() * constraint.covariance().inverse()
                  * deltaTrk)[0];
    }

    // assign new linearization point (= new vertex position in global frame)
    linPoint += deltaV;
    if (newChi2 < chi2) {
      chi2 = newChi2;

      Vector3D vertexPos(linPoint);

      fittedVertex.setPosition(vertexPos);
      fittedVertex.setCovariance(covDeltaVmat);
      fittedVertex.setFitQuality(chi2, ndf);

      std::vector<TrackAtVertex<InputTrack>> tracksAtVertex;

      std::shared_ptr<PerigeeSurface> perigee
          = Surface::makeShared<PerigeeSurface>(vertexPos);

      iTrack = 0;
      for (auto& bTrack : billoirTracks) {

        // new refitted trackparameters
        TrackParametersBase::ParVector_t paramVec;
        paramVec << 0., 0., trackMomenta[iTrack](0), trackMomenta[iTrack](1),
            trackMomenta[iTrack](2);

        BoundParameters refittedParams(
            std::move(covDeltaPmat[iTrack]), paramVec, perigee);

        TrackAtVertex<InputTrack> trackVx(
            bTrack.chi2, refittedParams, bTrack.originalTrack);
        tracksAtVertex.push_back(std::move(trackVx));
        ++iTrack;
      }
      fittedVertex.setTracksAtVertex(tracksAtVertex);
    }
  }  // end loop iterations
  return std::move(fittedVertex);
}