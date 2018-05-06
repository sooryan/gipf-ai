 %include "typemaps.i"
 %include "std_vector.i"
 %include "std_string.i"

 %template(FloatVector) std::vector<float>;

 %module gipf
 %{
 /* Includes the header in the wrapper code */
 #include "gtsa.hpp"
 #include "gipf.h"
 %}
 
 /* Parse the header file to generate wrappers */
 %include "gtsa.hpp"

 %template(VectorGipfMove) std::vector<GipfMove>;
 %template(BaseGipfMove) Move<GipfMove>;
 %template(BaseGipfState) State<GipfState, GipfMove>;

 %include "gipf.h"

%extend GipfState {
	std::string __str__() {
		ostringstream os;
		$self->to_stream(os);
		return os.str();
	}
}
