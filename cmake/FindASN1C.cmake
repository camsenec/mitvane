#  This file is copied
#  by Tomoya Tanaka <deepsky2221@gmail.com>
#  from <https://github.com/riebl/vanetza/blob/master/cmake/FindASN1C.cmake>
#  at 2022-02-25.
#  
#  This file is part of Mitvane.
# 
#  Mitvane is free software: you can redistribute it and/or modify it 
#  under the terms of the GNU Lesser General Public License as published
#  by the Free Software Foundation, either version 3 of the License, or 
#  any later version.
#  Mitvane is distributed in the hope that it will be useful, 
#  but WITHOUT ANY WARRANTY; without even the implied warranty of 
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
#  See the GNU General Public License and 
#  GNU Lesser General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License 
#  and GNU Lesser General Public License along with Mitvane. 
#  If not, see <https://www.gnu.org/licenses/>. 


find_program(ASN1C_EXECUTABLE NAMES asn1c DOC "ASN.1 compiler")
if(ASN1C_EXECUTABLE)
    execute_process(COMMAND ${ASN1C_EXECUTABLE} -version ERROR_VARIABLE _asn1c_version)
    string(REGEX MATCH "[0-9]\\.[0-9]\\.[0-9]+" ASN1C_VERSION "${_asn1c_version}")
    get_filename_component(_asn1c_executable_path ${ASN1C_EXECUTABLE} DIRECTORY)
endif()

find_path(ASN1C_SKELETON_DIR NAMES asn_application.c
    HINTS "${_asn1c_executable_path}/.."
    PATH_SUFFIXES share/asn1c skeletons
    DOC "Directory containing generic asn1c skeleton files")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ASN1C
    REQUIRED_VARS ASN1C_EXECUTABLE ASN1C_SKELETON_DIR
    FOUND_VAR ASN1C_FOUND
    VERSION_VAR ASN1C_VERSION)

mark_as_advanced(ASN1C_EXECUTABLE ASN1C_SKELETON_DIR)
