// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

///////////////////////////////////////////////////////////////////
// HomogeneousSurfaceMaterial.cpp, ACTS project
///////////////////////////////////////////////////////////////////

#include "ACTS/Material/HomogeneousSurfaceMaterial.hpp"
#include "ACTS/Material/MaterialProperties.hpp"

Acts::HomogeneousSurfaceMaterial::HomogeneousSurfaceMaterial() :
  Acts::SurfaceMaterial(),
  m_fullMaterial(0)
{}

Acts::HomogeneousSurfaceMaterial::HomogeneousSurfaceMaterial( const Acts::MaterialProperties& full,
                                                       double splitFactor) :
  Acts::SurfaceMaterial(splitFactor),
  m_fullMaterial(full.clone())
{}
                 
Acts::HomogeneousSurfaceMaterial::HomogeneousSurfaceMaterial(const Acts::HomogeneousSurfaceMaterial& lmp) :
  SurfaceMaterial(lmp.m_splitFactor),
  m_fullMaterial(lmp.m_fullMaterial ? lmp.m_fullMaterial->clone() : nullptr)
{}

Acts::HomogeneousSurfaceMaterial::~HomogeneousSurfaceMaterial()
{
  delete m_fullMaterial;        
}


Acts::HomogeneousSurfaceMaterial& Acts::HomogeneousSurfaceMaterial::operator=(const Acts::HomogeneousSurfaceMaterial& lmp)
{
  if (this!=&lmp) {    
    // first delete everything
    delete m_fullMaterial;
    // now refill evertything
    m_fullMaterial                      = lmp.m_fullMaterial ? lmp.m_fullMaterial->clone() : nullptr;
    Acts::SurfaceMaterial::m_splitFactor = lmp.m_splitFactor;
  }
  return (*this);
}

Acts::HomogeneousSurfaceMaterial& Acts::HomogeneousSurfaceMaterial::operator*=(double scale)
{
   // scale the sub properties    
   if (m_fullMaterial)  
      (*m_fullMaterial) *= scale;

   return (*this);
}

std::ostream& Acts::HomogeneousSurfaceMaterial::dump( std::ostream& sl) const
{
  sl << "Acts::HomogeneousSurfaceMaterial : " << std::endl;
  if (m_fullMaterial) {
       sl << "   - fullMaterial         : " << *m_fullMaterial << std::endl;
  }
  sl << "   - split factor         : " << m_splitFactor << std::endl; 
  return sl;
}