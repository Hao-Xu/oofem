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

#ifndef quad1mindlinshell3d_h
#define quad1mindlinshell3d_h

#include "nlstructuralelement.h"
#include "zznodalrecoverymodel.h"
#include "sprnodalrecoverymodel.h"

///@name Input fields for Quad1MindlinShell3D element
//@{
#define _IFT_Quad1MindlinShell3D_Name "quad1mindlinshell3d"
#define _IFT_Quad1MindlinShell3D_ReducedIntegration "reducedintegration"
#define _IFT_Quad1MindlinShell3D_mitc "mitc" ///< Mixed Interpolation Tensorial Components
//@}

namespace oofem {
class FEI2dQuadLin;

/**
 * This class implements an quadrilateral four-node shell element, using Mindlin plate theory.
 * Each node has 3 degrees of freedom (out-of-plane displacement, in-plane rotations).
 * This type of element exhibit strong shear locking (thin plates exhibit almost no bending).
 * No reduced integration is used, as it causes numerical problems.
 *
 * Loading types supported;
 * - Gravity load
 *
 * @note Status: Experimental.
 *
 * Reference:
 * Robert Cook, David Malkus, Michael Plesha
 * Concepts and Applications of Finite Element Analysis - Third edition
 * ISBN: 0-471-84788-7
 *
 * @author Mikael Öhman
 */
class Quad1MindlinShell3D : public NLStructuralElement,
public ZZNodalRecoveryModelInterface,
public SPRNodalRecoveryModelInterface
{
protected:
    /// Cached nodal coordinates in local c.s.,
    FloatArray *lnodes [ 4 ];
    /// Cached coordinates in local c.s.,
    FloatMatrix lcsMatrix;
    /// Flag controlling reduced (one - point) integration for shear
    bool reducedIntegrationFlag;

    static FEI2dQuadLin interp;

    /// Ordering for the normal shell stiffness (everything but the out-of-plane rotations)
    static IntArray shellOrdering;
    /// Ordering for the drilling dofs (the out-of-plane rotations)
    static IntArray drillOrdering;
    /// Dummy variable
    static bool __initialized;
    /// Defines the ordering of the dofs in the local stiffness matrix.
    static bool initOrdering() {
        shellOrdering = { 1, 2, 3, 4, 5, 7, 8, 9, 10, 11, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23};
        drillOrdering = { 6, 12, 18, 24};
        return true;
    }

public:
    Quad1MindlinShell3D(int n, Domain * d);
    virtual ~Quad1MindlinShell3D();

    virtual FEInterpolation *giveInterpolation() const;
    virtual FEInterpolation *giveInterpolation(DofIDItem id) const;

    virtual MaterialMode giveMaterialMode()  { return _3dShell; }
    virtual int giveApproxOrder() { return 1; }
    virtual int testElementExtension(ElementExtension ext) { return ( ( ext == Element_EdgeLoadSupport ) ? 1 : 0 ); }

    virtual void computeStiffnessMatrix(FloatMatrix &answer, MatResponseMode rMode, TimeStep *tStep);
    virtual void giveInternalForcesVector(FloatArray &answer, TimeStep *tStep, int useUpdatedGpRecord = 0);

    // definition & identification
    virtual const char *giveInputRecordName() const { return _IFT_Quad1MindlinShell3D_Name; }
    virtual const char *giveClassName() const { return "Quad1MindlinShell3D"; }
    virtual IRResultType initializeFrom(InputRecord *ir);

    virtual int computeNumberOfDofs() { return 24; }
    virtual int computeNumberOfGlobalDofs() { return 24; }
    virtual void giveDofManDofIDMask(int inode, EquationID, IntArray &) const;

    virtual void computeMidPlaneNormal(FloatArray &answer, const GaussPoint *gp);

    virtual double giveCharacteristicLenght(GaussPoint *gp, const FloatArray &normalToCrackPlane);
    virtual double computeVolumeAround(GaussPoint *gp);

    virtual void computeLumpedMassMatrix(FloatMatrix &answer, TimeStep *tStep);
    virtual void computeMassMatrix(FloatMatrix &answer, TimeStep *tStep)
    { computeLumpedMassMatrix(answer, tStep); }

    virtual int giveIPValue(FloatArray &answer, GaussPoint *gp, InternalStateType type, TimeStep *tStep);

    virtual void computeLCS();
    virtual bool computeGtoLRotationMatrix(FloatMatrix &answer);

    virtual Interface *giveInterface(InterfaceType it);

    virtual void SPRNodalRecoveryMI_giveSPRAssemblyPoints(IntArray &pap);
    virtual void SPRNodalRecoveryMI_giveDofMansDeterminedByPatch(IntArray &answer, int pap);
    virtual int SPRNodalRecoveryMI_giveNumberOfIP() { return this->numberOfGaussPoints; }
    virtual SPRPatchType SPRNodalRecoveryMI_givePatchType() { return SPRPatchType_2dxy; }
    virtual Element *ZZNodalRecoveryMI_giveElement() { return this; }




protected:
    virtual void computeGaussPoints();
    virtual void computeBodyLoadVectorAt(FloatArray &answer, Load *load, TimeStep *tStep, ValueModeType mode);
    virtual void computeSurfaceLoadVectorAt(FloatArray &answer, Load *load, int iSurf, TimeStep *tStep, ValueModeType mode);
    virtual void computeBmatrixAt(GaussPoint *gp, FloatMatrix &answer, int = 1, int = ALL_STRAINS);

    virtual void computeStrainVector(FloatArray &answer, GaussPoint *gp, TimeStep *tStep);
    virtual void computeStressVector(FloatArray &answer, const FloatArray &strain, GaussPoint *gp, TimeStep *tStep);
    virtual void computeConstitutiveMatrixAt(FloatMatrix &answer, MatResponseMode rMode, GaussPoint *gp, TimeStep *tStep);

    virtual void computeEgdeNMatrixAt(FloatMatrix &answer, int iedge, GaussPoint *gp);
    virtual void giveEdgeDofMapping(IntArray &answer, int iEdge) const;
    virtual double computeEdgeVolumeAround(GaussPoint *gp, int iEdge);
    virtual void computeEdgeIpGlobalCoords(FloatArray &answer, GaussPoint *gp, int iEdge);
    virtual int computeLoadLEToLRotationMatrix(FloatMatrix &answer, int iEdge, GaussPoint *gp);
    //virtual void computeSurfaceNMatrixAt(FloatMatrix &answer, GaussPoint *gp) { answer.clear(); }
    //virtual void giveSurfaceDofMapping(IntArray &answer, int iSurf) const { answer.clear(); }
    //virtual IntegrationRule *GetSurfaceIntegrationRule(int i) { return NULL; }
    //virtual double computeSurfaceVolumeAround(GaussPoint *gp, int iSurf) { return 0.; }
    //virtual void computeSurfIpGlobalCoords(FloatArray &answer, GaussPoint *gp, int iSurf) { answer.clear(); }
    void splitUnknowns(FloatArray &shellUnknowns, FloatArray &drillUnknowns, FloatArray &unknowns);
};
} // end namespace oofem
#endif // quad1mindlinshell3d_h
