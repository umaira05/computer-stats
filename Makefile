# Makefile for EECS 280 Machine Learning Project

CXX ?= g++
CXXFLAGS ?= -Wall -Werror -pedantic --std=c++17 -g


btlab_stats.exe: btlab_stats.cpp
	$(CXX) $(CXXFLAGS) btlab_stats.cpp -o btlab_stats.exe