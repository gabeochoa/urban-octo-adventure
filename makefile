MAKEFLAGS := --jobs=16
MAKEFLAGS += --output-sync=target

# FLAGS = -std=c++2a -stdlib=libc++ -Wall -Wextra -g -fsanitize=address
# FLAGS = -std=c++2a -Wall -Wextra -g -I/usr/local/include -stdlib=libc++ -fsanitize=address -fsanitize=undefined
# consider adding -Wfloat-equal
FLAGS = -std=c++2a -Wall -Wextra -Wpedantic -Wuninitialized -Wshadow -Wmost -g
INCLUDES = -Ivendor/ -Ivendor/LLGL/include -Ivendor/glm
LIBS = -Lvendor/LLGL/build/build -lLLGL -lLLGL_OpenGL
FRAMEWORKS = -framework OpenGL -framework Foundation -framework AppKit 

CCC = clang++


all: 
	$(CCC) $(FLAGS) $(INCLUDES) $(LIBS) $(FRAMEWORKS) test.cpp -o test.exe 


.PHONY: all 
