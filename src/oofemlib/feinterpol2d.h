/*
 *
 *                 #####    #####   ######  ######  ###   ###
 *               ##   ##  ##   ##  ##      ##      ## ### ##
 *              ##   ##  ##   ##  ####    ####    ##  #  ##
 *             ##   ##  ##   ##  ##      ##      ##     ##
 *            ##   ##  ##   ##  ##      ##      ##     ##
 *            #####    #####   ##      ######  ##     ##
 *
 *
 *             OOFEM : Object Oriented Finite Element Code
 *
 *               Copyright (C) 1993 - 2013   Borek Patzak
 *
 *
 *
 *       Czech Technical University, Faculty of Civil Engineering,
 *   Department of Structural Mechanics, 166 29 Prague, Czech Republic
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef feinterpol2d_h
#define feinterpol2d_h

#include "feinterpol.h"
#include "mathfem.h"

namespace oofem {
/**
 * Class representing a general abstraction for surface finite element interpolation class.
 */
class OOFEM_EXPORT FEInterpolation2d : public FEInterpolation
{
protected:
    int xind, yind;

public:
    FEInterpolation2d(int o, int ind1, int ind2) : FEInterpolation(o), xind(ind1), yind(ind2) { }

    virtual int giveNsd() { return 2; }

    /**
     * Computes the exact area.
     * @param cellgeo Cell geometry for the element.
     * @return Area of geometry.
     */
    virtual double giveArea(const FEICellGeometry &cellgeo) const;

    /**
     * Returns a characteristic length of the geometry, typically a diagonal or edge length.
     * @param cellgeo Underlying cell geometry.
     * @return Square root of area.
     */
    virtual double giveCharacteristicLength(const FEICellGeometry &cellgeo) const {return sqrt( this->giveArea(cellgeo) );}

    /**
     * Default implementation using Newton's method to find the local coordinates.
     * Can be overloaded if desired.
     */
    virtual int global2local(FloatArray &answer, const FloatArray &gcoords, const FEICellGeometry &cellgeo);

    virtual void giveJacobianMatrixAt(FloatMatrix &jacobianMatrix, const FloatArray &lcoords, const FEICellGeometry &cellgeo);

    virtual bool inside(const FloatArray &lcoords) const;

    virtual void boundaryEdgeGiveNodes(IntArray &answer, int boundary);
    virtual void boundaryEdgeEvalN(FloatArray &answer, int boundary, const FloatArray &lcoords, const FEICellGeometry &cellgeo);
    virtual double boundaryEdgeGiveTransformationJacobian(int boundary, const FloatArray &lcoords, const FEICellGeometry &cellgeo);
    virtual void boundaryEdgeLocal2Global(FloatArray &answer, int boundary, const FloatArray &lcoords, const FEICellGeometry &cellgeo);

    virtual void boundaryGiveNodes(IntArray &answer, int boundary);
    virtual void boundaryEvalN(FloatArray &answer, int boundary, const FloatArray &lcoords, const FEICellGeometry &cellgeo);
    virtual double boundaryEvalNormal(FloatArray &answer, int boundary, const FloatArray &lcoords, const FEICellGeometry &cellgeo);
    virtual double boundaryGiveTransformationJacobian(int boundary, const FloatArray &lcoords, const FEICellGeometry &cellgeo);
    virtual void boundaryLocal2Global(FloatArray &answer, int boundary, const FloatArray &lcoords, const FEICellGeometry &cellgeo);

    /**@name Edge interpolation services. */
    //@{
    virtual void computeLocalEdgeMapping(IntArray &edgeNodes, int iedge) = 0;
    void computeEdgeMapping(IntArray &edgeNodes, IntArray &elemNodes, int iedge);
    /**
     * Evaluates the array of edge interpolation functions (shape functions) at given point.
     * @param answer Contains resulting array of evaluated interpolation functions.
     * @param iedge Edge number.
     * @param lcoords Array containing (local) coordinates.
     * @param cellgeo Underlying cell geometry.
     */
    virtual void edgeEvalN(FloatArray &answer, int iedge, const FloatArray &lcoords, const FEICellGeometry &cellgeo) = 0;
    /**
     * Evaluates the normal on the given edge.
     * @param answer Contains the evaluated normal.
     * @param iedge Determines the edge number.
     * @param lcoords Array containing (local) coordinates.
     * @param cellgeo Underlying cell geometry.
     */
    virtual double edgeEvalNormal(FloatArray &answer, int iedge, const FloatArray &lcoords, const FEICellGeometry &cellgeo) = 0;
    /**
     * Evaluates the matrix of derivatives of edge interpolation functions (shape functions) at given point.
     * These derivatives are in global coordinate system (where the nodal coordinates are defined).
     * @param answer Contains resulting array of derivatives, the member at i position contains value of @f$ \frac{\mathrm{d}N_i}{\mathrm{d}s} @f$.
     * @param iedge Determines the edge number.
     * @param lcoords Array containing (local) coordinates.
     * @param cellgeo Underlying cell geometry.
     */
    virtual void edgeEvaldNds(FloatArray &answer, int iedge,
                              const FloatArray &lcoords,
                              const FEICellGeometry &cellgeo) = 0;
    /**
     * Evaluates edge global coordinates from given local ones.
     * These derivatives are in global coordinate system (where the nodal coordinates are defined).
     * @param answer Contains resulting global coordinates.
     * @param iedge Determines edge number.
     * @param lcoords Array containing (local) coordinates.
     * @param cellgeo Underlying cell geometry.
     */
    virtual void edgeLocal2global(FloatArray &answer, int iedge,
                                  const FloatArray &lcoords, const FEICellGeometry &cellgeo) = 0;
    /**
     * Evaluates the edge Jacobian of transformation between local and global coordinates.
     * @param iedge Determines edge number.
     * @param lcoords Array containing (local) coordinates.
     * @param cellgeo Underlying cell geometry.
     * @return Determinant of the mapping on the given edge.
     */
    virtual double edgeGiveTransformationJacobian(int iedge, const FloatArray &lcoords,
                                                  const FEICellGeometry &cellgeo);
    //@}

    virtual IntegrationRule *giveBoundaryIntegrationRule(int order, int boundary);
    virtual IntegrationRule *giveBoundaryEdgeIntegrationRule(int order, int boundary);
};
} // end namespace oofem
#endif // feinterpol2d_h
