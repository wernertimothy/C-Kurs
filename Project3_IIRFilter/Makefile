# Weitere Erklärungen unter:
# http://www.ijon.de/comp/tutorials/makefile.html
# Name der ausführbaren Datei 
TARGET = PrjDay6

# zu kompilierende Source Dateien
# Beachte: keine Leerzeichen nach dem Backslash. Der Backslash wandelt den Folgenden Zeilenumbruch für das make Programm in ein gewöhnliches weißes Zeichen um
CSRC = 	main.c	\
		
# Suchpfade für Header-Dateien
INCDIRS = include
# Suchpfade für eigene Bibliotheken
LIBDIRS = lib
# Verwendete eigene Bibliotheken
LIBS = matrixmath


# Compiler (CFLAGS) und Linker (LDFLAGS) Flags zentral definieren
# -Wall   : bei allen Warnungen abbrechen
# -Wextra : besonders pingelig warnen
# -Wpeadantic	: und auch noch viel pingeliger warnen for nicht iso-konformen Konstrukten
# -g3     : Maximalmenge an Debug-Infos in die .exe packen
LDFLAGS = -Wpedantic -Wall -Wextra -g3
CFLAGS  = -Wpedantic -Wall -Wextra -g3

# Wahl der gcc Version (Name der .exe)
CC = gcc

# Namen der .o Dateien aus den .c-Dateien erzeugen
# https://www.gnu.org/software/make/manual/make.html#Substitution-Refs
COBJ = $(subst .c,.o,$(CSRC))

# inc-Verzeichnisse um -I Option ergänzen
# https://www.gnu.org/software/make/manual/make.html#Foreach-Function
INC_PARAMS = $(foreach d, $(INCDIRS), -I$d)
# Bibliotheksverzeichnisse um -L Option ergänzen
LIBDIR_PARAMS = $(foreach d, $(LIBDIRS), -L$d)
# Bibliotheken um -l Option ergänzen
LIB_PARAMS = $(foreach d, $(LIBS),-l$d)


# erzeuge ausführbare Datei
all: $(TARGET)

# linke alle .o-Dateien
# "$^" - Namen aller Prerequisites (Deutsch: Vorausgesetzten Dateien, also die ganzen Objekt-Dateien) ersetzt
# "$@" - Target Name der aktuellen Regel. In diesem Fall also "$(Target)"  
$(TARGET): $(COBJ)
	$(CC) $(LDFLAGS) $(LIBDIR_PARAMS) $^ -o $@ $(LIB_PARAMS)

# erzeuge alle .o Dateien
# https://www.gnu.org/software/make/manual/make.html#Pattern-Rules
# "%.o"	: passt auf alle Dateien mit der .o-Endung. Als Vorraussetzung wird die zugehörige .c-Datei gelistet
# "$<"	: Name der ersten Prerequisite (erste vorrausgesetzte Datei). In diesem Fall gibt es auch immer nur eine vorrausgesetzte Datei
# "$@"	: Name des Targets, hier wird das zu der Datei mit der Endung .o (das Target wurde als "%.o" definiert)
# -c	: kompilieren aber nicht linken --> Object-files 
%.o: %.c
	$(CC) $(CFLAGS) $(INC_PARAMS)  $< -c -o $@ 

# all und clean sind keine Datei-Ziele
# http://www.ijon.de/comp/tutorials/makefile.html#phony
# .PHONY sorgt dafür, dass die danach folgenden Targets immer ausgeführt werden
.PHONY: all clean

# alle erzeugten object files und ausführbaren Dateien wieder löschen
clean:
	rm -rf $(COBJ) $(TARGET)