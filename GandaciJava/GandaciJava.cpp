#include <cstring>
#include <fstream>
#include <vector>

// Manages researcher-bug pairs. Used by the GandaciJava class.
class PairManager
{
    private:
        short researcherCount;
        short bugCount;
        short researcher[10001];
        short bug[10001];

    public:
        void initialize(short researcherCount, short bugCount)
        {
            this -> researcherCount = researcherCount;
            this -> bugCount = bugCount;

            // Clears the researcher and bug arrays.
            memset(researcher, 0, sizeof(short) * (researcherCount + 1));
            memset(bug, 0, sizeof(short) * (bugCount + 1));
        }

        void pair(short researcher, short bug)
        {
            this -> researcher[researcher] = bug;
            this -> bug[bug] = researcher;
        }

        // Determines whether a specified researcher is paired with a bug.
        bool isResearcherPaired(short researcher)
        {
            // Returns true if researcher[researcher] != 0.
            return this -> researcher[researcher];
        }

        // Determines whether a specified bug is paired with a researcher.
        bool isBugPaired(short bug)
        {
            // Returns true if bug[bug] != 0.
            return this -> bug[bug];
        }

        short getResearcherOfBug(short bug)
        {
            return this -> bug[bug];
        }

        short getBugOfResearcher(short researcher)
        {
            return this -> researcher[researcher];
        }

        // Counts the number of pairs.
        short getPairCount()
        {
            short pairCount = 0;

            for (short bugIndex = 1; bugIndex <= bugCount; bugIndex++)
            {
                // Determines whether this bug is paired with a researcher.
                if (bug[bugIndex])
                {
                    pairCount++;
                }
            }

            return pairCount;
        }
};

// Keeps track of researcher "visits". Used by the GandaciJava class.
class VisitManager
{
    private:
        bool visitedResearchers[10001];

    public:
        void initialize(short researcherCount)
        {
            // Clears the visitedResearchers array.
            memset(visitedResearchers, 0, sizeof(bool) * (researcherCount + 1));
        }

        bool isVisited(short researcher)
        {
            return visitedResearchers[researcher];
        }

        void visit(short researcher)
        {
            visitedResearchers[researcher] = true;
        }
};

// Represents a test. Used by the GandaciJava class.
class Test
{
    private:
        short researcherCount;
        short bugCount;
        std::vector<short> *experiment;

    public:
        Test(short researcherCount, short bugCount)
        {
            this -> researcherCount = researcherCount;
            this -> bugCount = bugCount;

            // Initializes the experiment arrays.
            experiment = new std::vector<short>[researcherCount + 1];
        }

        short getResearcherCount()
        {
            return researcherCount;
        }

        short getBugCount()
        {
            return bugCount;
        }

        void addExperiment(short researcher, short bug)
        {
            if (experiment[researcher].size() % 10 == 0)
            {
                experiment[researcher].reserve(experiment[researcher].size() + 10);
            }

            experiment[researcher].push_back(bug);
        }

        // Returns an array that points at every experiment.
        short *getExperiments(short researcher)
        {
            if (experiment[researcher].empty())
            {
                return NULL;
            }
            else
            {
                // Elements in a vector are guaranteed to be contiguous,
                // therefore this is a valid conversion.
                return &experiment[researcher][0];
            }
        }

        short getExperimentCount(short researcher)
        {
            return experiment[researcher].size();
        }
};

// Solves the "Gandaci Java" problem from InfoArena:
// http://www.infoarena.ro/problema/java
class GandaciJava
{
    private:
        PairManager pairManager;
        VisitManager visitManager;
        Test *currentTest;
        std::vector<short> solutions;

        // Attempts to pair a researcher. Optimizes the other pairs while doing so.
        // Returns a boolean value which determines whether it succeeded to pair the specified researcher.
        bool pair(short researcher)
        {
            // Gets the experiments of this researcher.
            short *experiments = currentTest -> getExperiments(researcher);
            short experimentCount = currentTest -> getExperimentCount(researcher);

            // Researchers which have been visited before should not be visited again,
            // therefore this is being kept track of:
            visitManager.visit(researcher);

            // Determines whether there are any bugs which aren't paired with a researcher.
            for (short experimentIndex = 0; experimentIndex < experimentCount; experimentIndex++)
            {
                // Gets the bug from this experiment.
                short bug = experiments[experimentIndex];

                // Determines whether this bug is paired with a researcher.
                if (!pairManager.isBugPaired(bug))
                {
                    pairManager.pair(researcher, bug);

                    // This researcher has been paired.
                    return true;
                }
            }

            // Determines whether there is any researcher-bug pair which can be changed.
            for (short experimentIndex = 0; experimentIndex < experimentCount; experimentIndex++)
            {
                // Gets the bug from this experiment.
                short bug = experiments[experimentIndex];

                // Gets the researcher paired with this bug.
                short anotherResearcher = pairManager.getResearcherOfBug(bug);

                // Has anotherResearcher been visited before?
                if (!visitManager.isVisited(anotherResearcher))
                {
                    // If it hasn't, then perform this function on it as well.
                    if (pair(anotherResearcher))
                    {
                        // If anotherResearcher could be paired with a different bug,
                        // then pair this researcher with this bug.
                        pairManager.pair(researcher, bug);

                        // This researcher has been paired.
                        return true;
                    }
                }
            }

            // This researcher has not been paired.
            return false;
        }

        // Finds a matching of researchers and bugs that contains the maximum number of bugs.
        void maximumBipartiteGraphMatching()
        {
            // Gets the number of researchers.
            short researcherCount = currentTest -> getResearcherCount();

            // Gets the number of bugs.
            short bugCount = currentTest -> getBugCount();

            // Initializes the PairManager.
            pairManager.initialize(researcherCount, bugCount);

            bool atLeastOneResearcherWasPaired;

            do
            {
                atLeastOneResearcherWasPaired = false;

                // Reinitializes the VisitManager for this loop.
                visitManager.initialize(researcherCount);

                for (short researcher = 1; researcher <= researcherCount; researcher++)
                {
                    if (!pairManager.isResearcherPaired(researcher) && pair(researcher))
                    {
                        atLeastOneResearcherWasPaired = true;
                    }
                }

            } while (atLeastOneResearcherWasPaired);
        }

    public:
        // Loads a set of tests from a specified path.
        std::vector<Test*> loadTests(std::string path)
        {
            std::ifstream file(path);
            std::vector<Test*> tests;
            short testCount;

            // Reads the number of tests.
            file >> testCount;
            tests.resize(testCount);

            for (short testIndex = 0; testIndex < testCount; testIndex++)
            {
                short researcherCount;
                short bugCount;
                int experimentCount;

                // Reads the number of researchers.
                file >> researcherCount;
                
                // Reads the number of bugs.
                file >> bugCount;
                
                // Reads the number of experiments.
                file >> experimentCount;

                // Creates a new test.
                tests[testIndex] = new Test(researcherCount, bugCount);

                // Reads the experiments.
                while (experimentCount--)
                {
                    short researcher;
                    short bug;

                    // Reads the experiment data.
                    file >> researcher;
                    file >> bug;

                    // Adds this experiment.
                    tests[testIndex] -> addExperiment(researcher, bug);
                }
            }

            return tests;
        }

        // Solves the specified test.
        void solve(Test *test)
        {
            currentTest = test;

            // Finds a matching of researchers and bugs that contains the maximum number of bugs.
            maximumBipartiteGraphMatching();

            // Stores the solution.
            solutions.push_back(pairManager.getPairCount());
        }

        // Solves an array of tests.
        void solve(std::vector<Test*> tests)
        {
            for (Test *test : tests)
            {
                solve(test);
            }
        }

        // Writes the solution for each test to the specified path.
        void writeSolution(std::string path)
        {
            std::ofstream file(path);

            for (short solution : solutions)
            {
                file << solution << '\n';
            }
        }
};

int main()
{
    // Creates an instance of the GandaciJava class.
    GandaciJava gandaciJava;

    // Loads the tests.
    std::vector<Test*> tests = gandaciJava.loadTests("java.in");

    // Performs magic.
    gandaciJava.solve(tests);

    // Writes the solutions.
    gandaciJava.writeSolution("java.out");

    return 0;
}