# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g

# MySQL libraries and includes
MYSQL_INCLUDE = -I/usr/include/mysql
MYSQL_LIB = -L/usr/lib/mysql -lmysqlclient

# Source and output files
SRC = my_sql_sample.cpp
OBJ = my_sql_sample.o
EXEC = my_sql_sample

# The default target
all: $(EXEC)

# Rule to build the executable
$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $(EXEC) $(MYSQL_LIB)

# Rule to compile source file into object file
$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) $(MYSQL_INCLUDE) -c $(SRC)

# Clean up object and executable files
clean:
	rm -f $(OBJ) $(EXEC)

# Install (optional)
install: $(EXEC)
	# (Add installation steps if necessary)

# Run the program
run: $(EXEC)
	./$(EXEC)

# Phony targets
.PHONY: all clean install run

