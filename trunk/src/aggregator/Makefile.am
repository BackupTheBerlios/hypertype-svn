INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO
if ENABLE_AGGREGATOR
AGGREGATOR_APP = hypertype-aggregator
else
AGGREGATOR_APP =
endif

bin_PROGRAMS = $(AGGREGATOR_APP)
hypertype_aggregator_SOURCES = wxaggregator.cpp wxaggregator.h grid.cpp \
				aggregator_engine.cpp string_aid.cpp widget_settings.cpp total_aid.cpp grid_wx.cpp \
		typing_history_aggregator.cpp time_aid.cpp file_aid.cpp
AM_CXXFLAGS = `wx-config --cxxflags --unicode=no` -I/usr/include/libxml2
hypertype_aggregator_LDFLAGS = `wx-config --libs --unicode=no` -L/usr/lib \
	/usr/lib/libxml2.so
noinst_HEADERS = grid.h aggregator_engine.h total_aid.h grid_wx.h \
		typing_history_aggregator.h time_aid.h

