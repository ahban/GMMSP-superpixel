% Please make sure you have execute the following command successfully.
%   mex -setup c++ 
% Because sort needs a complexity of O(N log(N)), we prefer a C++ 11 compiler.
%   see http://en.cppreference.com/w/cpp/algorithm/sort

% Windows
if ispc
  mex -O -I. COMPFLAGS="/openmp $COMPFLAGS" mx_GMMSP.cpp ./sp/CISP.cpp
  
% Linux
elseif isunix
  mex -O -I. CXXFLAGS='-w -fopenmp $CXXFLAGS' LDFLAGS='-w -fopenmp $LDFLAGS' mx_GMMSP.cpp ./sp/CISP.cpp 
end

%copyfile *.mex* ../bin