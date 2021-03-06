/*!
Copyright (C) 2014, 申瑞珉 (Ruimin Shen)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cassert>
#include <random>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/math/constants/constants.hpp>
#include <OTL/Problem/DTLZ/DTLZ2.h>
#include <OTL/Crossover/CoupleCoupleCrossoverAdapter.h>
#include <OTL/Crossover/Real/SBX/SimulatedBinaryCrossover.h>
#include <OTL/Initial/Real/Uniform.h>
#include <OTL/Mutation/Real/PM/PolynomialMutation.h>
#include <OTL/Optimizer/NSGA-III/NSGA-III.h>
#include <OTL/Utility/Weight/NormalBoundaryIntersection.h>

namespace nsga_iii
{
BOOST_AUTO_TEST_CASE(NSGA_III)
{
	typedef std::mt19937 _TRandom;
	typedef double _TReal;
	typedef otl::problem::dtlz::DTLZ2<_TReal> _TProblem;
	typedef _TProblem::TDecision _TDecision;
	typedef otl::crossover::real::sbx::SimulatedBinaryCrossover<_TReal, _TRandom &> _TCrossover;
	typedef otl::mutation::real::pm::PolynomialMutation<_TReal, _TRandom &> _TMutation;
	typedef otl::optimizer::nsga_iii::NSGA_III<_TReal, _TDecision, _TRandom &> _TOptimizer;
	const size_t nObjectives = 3;
	_TRandom random;
	_TProblem problem(nObjectives);
	std::list<std::vector<_TReal> > points = otl::utility::weight::NormalBoundaryIntersection<_TReal>(nObjectives, 12);
	std::vector<std::vector<_TReal> > referenceSet(points.begin(), points.end());
	size_t nPopulation = referenceSet.size();
	nPopulation += (4 - nPopulation % 4) % 4;
	const std::vector<_TDecision> initial = otl::initial::real::BatchUniform(random, problem.GetBoundary(), nPopulation);
	_TCrossover _crossover(random, 1, problem.GetBoundary(), 20);
	otl::crossover::CoupleCoupleCrossoverAdapter<_TReal, _TDecision, _TRandom &> crossover(_crossover, random);
	_TMutation mutation(random, 1 / (_TReal)problem.GetBoundary().size(), problem.GetBoundary(), 20);
	_TOptimizer optimizer(random, problem, initial, crossover, mutation, referenceSet);
	BOOST_CHECK(problem.GetNumberOfEvaluations() == nPopulation);
	for (size_t generation = 1; problem.GetNumberOfEvaluations() < 30000; ++generation)
	{
		optimizer();
		BOOST_CHECK_EQUAL(problem.GetNumberOfEvaluations(), (generation + 1) * nPopulation);
	}
}
}
