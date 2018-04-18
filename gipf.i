 %include "typemaps.i"
 %include "std_vector.i"
 %module(directors="1") gipf
 %{
 /* Includes the header in the wrapper code */
 #include "gtsa.hpp"
 #include "gipf.h"
 %}
 
 %template(FloatVector) std::vector<float>;
 /* Parse the header file to generate wrappers */
 %include "gtsa.hpp"
 %include "gipf.h"

%template(VectorGipfMove) std::vector<GipfMove>;