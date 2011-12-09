file(GLOB hpp_files *.hpp)



# make the header public
pods_install_headers(${hpp_files} DESTINATION stl_utils)


