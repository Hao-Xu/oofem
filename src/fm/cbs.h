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

#ifndef cbs_h
#define cbs_h

#include "fluidmodel.h"
#include "sparselinsystemnm.h"
#include "sparsemtrx.h"
#include "primaryfield.h"
//<RESTRICTED_SECTION>
#include "materialinterface.h"
//</RESTRICTED_SECTION>

#include "unknownnumberingscheme.h"

///@name Input fields for CBS
//@{
#define _IFT_CBS_Name "cbs"
#define _IFT_CBS_deltat "deltat"
#define _IFT_CBS_mindeltat "mindeltat"
#define _IFT_CBS_cmflag "cmflag"
#define _IFT_CBS_theta1 "theta1"
#define _IFT_CBS_theta2 "theta2"
#define _IFT_CBS_scaleflag "scaleflag"
#define _IFT_CBS_lscale "lscale"
#define _IFT_CBS_uscale "uscale"
#define _IFT_CBS_dscale "dscale"
#define _IFT_CBS_miflag "miflag"
//@}

namespace oofem {
/**
 * Specialized numbering scheme for CBS algorithm, since it needs velocities separately.
 */
class VelocityEquationNumbering : public UnknownNumberingScheme
{
protected:
    bool prescribed;
    int numEqs;

public:
    VelocityEquationNumbering(bool prescribed) : UnknownNumberingScheme(), prescribed(prescribed), numEqs(0) { }

    virtual bool isDefault() const { return !prescribed; }
    virtual int giveDofEquationNumber(Dof *dof) const {
        DofIDItem id = dof->giveDofID();
        if ( id == V_u || id == V_v || id == V_w ) {
            return prescribed ? dof->__givePrescribedEquationNumber() : dof->__giveEquationNumber();
        }

        return 0;
    }
    virtual int giveRequiredNumberOfDomainEquation() const { return numEqs; }

    int askNewEquationNumber() { return ++numEqs; }
};

/**
 * Specialized numbering scheme for CBS algorithm, since it needs pressures separately.
 */
class PressureEquationNumbering : public UnknownNumberingScheme
{
protected:
    bool prescribed;
    int numEqs;

public:
    PressureEquationNumbering(bool prescribed) : UnknownNumberingScheme(), prescribed(prescribed), numEqs(0) { }

    virtual bool isDefault() const { return !prescribed; }
    virtual int giveDofEquationNumber(Dof *dof) const {
        DofIDItem id = dof->giveDofID();
        if ( id == P_f ) {
            return prescribed ? dof->__givePrescribedEquationNumber() : dof->__giveEquationNumber();
        }

        return 0;
    }
    virtual int giveRequiredNumberOfDomainEquation() const { return numEqs; }

    int askNewEquationNumber() { return ++numEqs; }
};

/**
 * This class represents CBS algorithm for solving incompressible Navier-Stokes equations
 */
class CBS : public FluidModel
{
protected:
    /// Numerical method used to solve the problem.
    SparseLinearSystemNM *nMethod;

    LinSystSolverType solverType;
    SparseMtrxType sparseMtrxType;

    SparseMtrx *lhs;
    /// Pressure field.
    PrimaryField PressureField;
    /// Velocity field.
    PrimaryField VelocityField;
    FloatArray deltaAuxVelocity;
    FloatArray prescribedTractionPressure;
    FloatArray nodalPrescribedTractionPressureConnectivity;

    /// Lumped mass matrix.
    FloatArray mm;
    /// Sparse consistent mass.
    SparseMtrx *mss;

    /// Time step and its minimal value.
    double deltaT, minDeltaT;
    /// Integration constants.
    double theta1, theta2;

    int initFlag;
    /// Consistent mass flag.
    int consistentMassFlag;

    VelocityEquationNumbering vnum;
    VelocityEquationNumbering vnumPrescribed;
    PressureEquationNumbering pnum;
    PressureEquationNumbering pnumPrescribed;

    bool equationScalingFlag;
    /// Length scale.
    double lscale;
    /// Velocity scale.
    double uscale;
    /// Density scale.
    double dscale;
    /// Reynolds number
    double Re;

    //<RESTRICTED_SECTION>
    // material interface representation for multicomponent flows
    MaterialInterface *materialInterface;
    //</RESTRICTED_SECTION>
public:
    CBS(int i, EngngModel * _master = NULL) : FluidModel(i, _master),
        PressureField(this, 1, FT_Pressure, EID_ConservationEquation, 1),
        VelocityField(this, 1, FT_Velocity, EID_MomentumBalance, 1),
        vnum(false), vnumPrescribed(true), pnum(false), pnumPrescribed(true) {
        initFlag = 1;
        lhs = NULL;
        ndomains = 1;
        nMethod = NULL;
        consistentMassFlag = 0;
        equationScalingFlag = false;
        lscale = uscale = dscale = 1.0;
        //<RESTRICTED_SECTION>
        materialInterface = NULL;
        //</RESTRICTED_SECTION>
    }
    virtual ~CBS() {
        //<RESTRICTED_SECTION>
        if ( materialInterface ) {
            delete materialInterface;
        }

        //</RESTRICTED_SECTION>
    }

    virtual void solveYourselfAt(TimeStep *tStep);

    virtual void updateYourself(TimeStep *tStep);

    virtual double giveUnknownComponent(ValueModeType type, TimeStep *tStep, Domain *d, Dof *dof);
    virtual double giveReynoldsNumber();
    double giveTheta1();
    double giveTheta2();
    double giveTractionPressure(Dof *dof);

    virtual contextIOResultType saveContext(DataStream &stream, ContextMode mode);
    virtual contextIOResultType restoreContext(DataStream &stream, ContextMode mode);

    virtual void updateDomainLinks();

    virtual TimeStep *giveNextStep();
    virtual TimeStep *giveSolutionStepWhenIcApply();
    virtual NumericalMethod *giveNumericalMethod(MetaStep *mStep);

    virtual IRResultType initializeFrom(InputRecord *ir);

    virtual int checkConsistency();
    // identification
    virtual const char *giveClassName() const { return "CBS"; }
    virtual const char *giveInputRecordName() const { return _IFT_CBS_Name; }
    virtual fMode giveFormulation() { return TL; }

    virtual void printDofOutputAt(FILE *stream, Dof *iDof, TimeStep *tStep);

    virtual int giveNumberOfDomainEquations(int, const UnknownNumberingScheme &num);

    virtual int giveNewEquationNumber(int domain, DofIDItem);
    virtual int giveNewPrescribedEquationNumber(int domain, DofIDItem);

    virtual bool giveEquationScalingFlag() { return equationScalingFlag; }
    virtual double giveVariableScale(VarScaleType varId);

protected:
    /**
     * Updates nodal values
     * (calls also this->updateDofUnknownsDictionary for updating dofs unknowns dictionaries
     * if model supports changes of static system). The element internal state update is also forced using
     * updateInternalState service.
     */
    void updateInternalState(TimeStep *tStep);
    void applyIC(TimeStep *tStep);
};
} // end namespace oofem
#endif // cbs_h
