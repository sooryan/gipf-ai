 %include "typemaps.i"
 %include "std_vector.i"
 %include "std_string.i"
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

%extend GipfState {
	std::string __str__() {
		ostringstream os;
		$self->to_stream(os);
		return os.str();
	}
}
