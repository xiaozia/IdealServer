add_executable(sudoku_solver_basic server_basic.cc sudoku.cc)
target_link_libraries(sudoku_solver_basic ideal_net ideal_base pthread)

add_executable(sudoku_solver_threadpool server_threadpool.cc sudoku.cc)
target_link_libraries(sudoku_solver_threadpool ideal_net ideal_base pthread)

add_executable(sudoku_solver_multiloop server_multiloop.cc sudoku.cc)
target_link_libraries(sudoku_solver_multiloop ideal_net ideal_base pthread)

add_executable(sudoku_loadtest loadtest.cc sudoku.cc)
target_link_libraries(sudoku_loadtest ideal_net ideal_base)

