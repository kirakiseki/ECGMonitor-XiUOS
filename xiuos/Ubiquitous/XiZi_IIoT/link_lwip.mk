OBJS := $(shell cat make.obj)

$(TARGET): $(OBJS) 
	@echo ------------------------------------------------
	@echo link $(TARGET)
	@$(CROSS_COMPILE)ar -r $@  $(OBJS)
	@echo ------------------------------------------------
	@$(CROSS_COMPILE)objcopy $@ $(TARGET)
	@$(CROSS_COMPILE)size $@