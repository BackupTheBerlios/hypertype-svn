bin_PROGRAMS = hypertype
hypertype_SOURCES = hypertype.cpp string_aid.cpp string_aid.h widget.cpp \
	widget.h array_aid.cpp array_aid.h SDL_aid.cpp SDL_aid.h widget_settings.cpp \
	widget_settings.h xml_aid.h widget_button.cpp widget_image.cpp widget_text.cpp \
	widget_newkeys.cpp widget_accuracy.cpp file_aid.cpp widget_practice.cpp widget_practice.h \
	widget_display.cpp screen.cpp menu.cpp lesson.cpp lesson.h message_box.cpp message_box.h \
	widget_keyboard.cpp widget_completion.cpp widget_speed.cpp game.cpp game.h \
	text_generation.cpp widget_lesson_progress.cpp OS.cpp globals.cpp font_aid.cpp \
	widget_settings_file.cpp image_aid.cpp

# set the include path found by configure
INCLUDES= $(all_includes)

# the library search path.
hypertype_LDFLAGS = $(all_libraries) -L/usr/lib /usr/lib/libxml2.so /usr/lib/libSDL_image.so /usr/lib/libSDL_ttf.so

AM_CXXFLAGS = -I/usr/include/libxml2 -I/usr/include/SDL -DDATA_PREFIX=\"\$(datadir)\"
hypertype_LDADD = -L/usr/lib /usr/lib/libSDL.so
noinst_HEADERS = xml_aid.h widget_button.h widget_image.h widget_text.h \
						widget_text.h widget_newkeys.h message_box.h widget_accuracy.h file_aid.h file_aid.h \
						precompiled.h widget_display.h screen.h menu.h widget_keyboard.h widget_completion.h \
						widget_speed.h text_generation.h widget_lesson_progress.h OS.h globals.h font_aid.h \
		widget_settings_file.h image_aid.h


SUBDIRS = aggregator data
