ifeq ($(CONFIG_USING_CMSIS_5),y)
	SRC_FILES := $(wildcard NN/Source/ActivationFunctions/*.c) $(wildcard NN/Source/BasicMathFunctions/*.c) $(wildcard NN/Source/ConcatenationFunctions/*.c)\
	$(wildcard NN/Source/ConvolutionFunctions/*.c) $(wildcard NN/Source/FullyConnectedFunctions/*.c) $(wildcard NN/Source/NNSupportFunctions/*.c) \
	$(wildcard NN/Source/PoolingFunctions/*.c) $(wildcard NN/Source/ReshapeFunctions/*.c) $(wildcard NN/Source/SoftmaxFunctions/*.c) $(wildcard NN/Source/SVDFunctions/*.c)
endif


include $(KERNEL_ROOT)/compiler.mk
