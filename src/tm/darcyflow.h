/*
 * DarcyFlow.h
 *
 *  Created on: Feb 25, 2010
 *      Author: carl
 */

#ifndef darcyflow_h
#define darcyflow_h

#include "engngm.h"
#include "inputrecord.h"
#include "sparsemtrxtype.h"
#include "linsystsolvertype.h"
#include "sparselinsystemnm.h"
#include "sparsenonlinsystemnm.h"

#define _IFT_DarcyFlow_Name "darcyflow"

namespace oofem {
class CommunicatorBuff;
class ProblemCommunicator;

/**
 * Class describing an extended Darcy flow. A Darcy flow is a linear relation between the seepage velocity and the pressure gradient. By 'extended',
 * we imply that there is a possibility for a nonlinear relation.
 * @author Carl Sandström
 */
class DarcyFlow : public EngngModel
{
private:
    LinSystSolverType solverType;

protected:
    PrimaryField *PressureField;
    SparseMtrxType sparseMtrxType;
    SparseNonLinearSystemNM *nMethod;

    SparseMtrx *stiffnessMatrix;
    FloatArray internalForces;
    FloatArray externalForces;
    FloatArray incrementOfSolution;

    int currentIterations;
    FloatArray ebeNorm;
    FloatArray incrementOfDisplacement;
    FloatArray incrementalBCLoadVector;
    FloatArray incrementalLoadVector;
    FloatArray initialLoad;
    SparseNonLinearSystemNM :: referenceLoadInputModeType refLoadInputMode;
    bool hasAdvanced;

public:
    DarcyFlow(int i, EngngModel * _master);
    virtual ~DarcyFlow();

    virtual void solveYourselfAt(TimeStep *tStep);
    virtual void updateYourself(TimeStep *tStep);

    virtual contextIOResultType saveContext(DataStream &stream, ContextMode mode) { return CIO_IOERR; };
    virtual contextIOResultType restoreContext(DataStream &stream, ContextMode mode) { return CIO_IOERR; };
    virtual int checkConsistency() { return 1; };
    virtual fMode giveFormulation() { return TL; }

    virtual void updateComponent(TimeStep *tStep, NumericalCmpn cmpn, Domain *d);

    virtual double giveUnknownComponent(ValueModeType, TimeStep *, Domain *, Dof *);

    virtual IRResultType initializeFrom(InputRecord *ir);
    virtual void printDofOutputAt(FILE *stream, Dof *iDof, TimeStep *tStep);
    void DumpMatricesToFile(FloatMatrix *LHS, FloatArray *RHS, FloatArray *SolutionVector);

    virtual NumericalMethod *giveNumericalMethod(MetaStep *mStep);
    virtual TimeStep *giveNextStep();

    virtual int forceEquationNumbering(int id);

    virtual const char *giveInputRecordName() const { return _IFT_DarcyFlow_Name; }
    virtual const char *giveClassName() const { return "DarcyFlow"; }

#ifdef __PARALLEL_MODE
    CommunicatorBuff *commBuff; //new CommunicatorBuff(this->giveNumberOfProcesses(), CBT_static);
    ProblemCommunicator *communicator;
#endif

#ifdef __PARALLEL_MODE
    virtual void initParallelContexts();
#endif
};
}

#endif // darcyflow_h
