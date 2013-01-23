MAPPING_INC = -I$(SRC_SUBSYSTEMS)/.. -I$(PAPARAZZI_SRC)/sw/include -I$(SRC_ARCH)

ap.CFLAGS += -DUSE_MAPPING

ap.CFLAGS += -DMAPPING_TYPE_H=\"subsystems/mapping/mapping_navigation.h\"

$(TARGET).CXXFLAGS += $(MAPPING_INC)

$(TARGET).srcs += $(SRC_SUBSYSTEMS)/mapping/mapping_navigation.c

$(TARGET).cpp_srcs += $(SRC_SUBSYSTEMS)/mapping/mapping_wrapper.cpp 
$(TARGET).cpp_srcs += $(SRC_SUBSYSTEMS)/mapping/Node.cpp
$(TARGET).cpp_srcs += $(SRC_SUBSYSTEMS)/mapping/Map2D.cpp
$(TARGET).cpp_srcs += $(SRC_SUBSYSTEMS)/mapping/Frontier.cpp
$(TARGET).cpp_srcs += $(SRC_SUBSYSTEMS)/mapping/Astar.cpp
$(TARGET).cpp_srcs += $(SRC_SUBSYSTEMS)/mapping/MapPos.cpp
$(TARGET).cpp_srcs += $(SRC_SUBSYSTEMS)/mapping/Mapping.cpp
