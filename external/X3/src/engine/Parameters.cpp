#include <Parameters.h>

namespace X3
{
    Parameters& Parameters::Inst()
    {
        static Parameters p;
        return p;
    }
}
