
all:
ifeq ($(CONFIG_SENSOR_BME680), y)
	$(MAKE) -C bme680 all
endif

clean:
ifeq ($(CONFIG_SENSOR_BME680), y)
	$(MAKE) -C bme680 clean
endif
