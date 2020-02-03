
all:
ifeq ($(CONFIG_SENSOR_BME680), y)
	$(MAKE) -C bme680 all
endif

ifeq ($(CONFIG_SENSOR_PMSA003), y)
	$(MAKE) -C pmsa003 all
endif


clean:
ifeq ($(CONFIG_SENSOR_BME680), y)
	$(MAKE) -C bme680 clean
endif

ifeq ($(CONFIG_SENSOR_PMSA003), y)
	$(MAKE) -C pmsa003 clean
endif
