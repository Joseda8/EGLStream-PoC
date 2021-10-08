INCDIR=-I./Common
LIBS=-lGLESv2 -lEGL -lm -lX11 -lancillary 

COMMONSRC=./Common/esShader.c    \
          ./Common/esTransform.c \
          ./Common/esShapes.c

COMMONHRD=esUtil.h

UTILBASE=./Base/esUtil.c
UTILCONSUMER=./Consumer/esUtil.c
UTILPRODUCER=./Producer/esUtil.c

SRC_B=./Base/base.c
SRC_C=./Consumer/consumer.c
SRC_P=./Producer/producer.c

default: all

all:                                      \
     ./Base/base  \
     ./Consumer/consumer                  \
     ./Producer/producer

clean:
	rm ./Consumer/consumer ./Producer/producer ./Base/base

./Base/base: ${UTILBASE} ${COMMONHDR} ${SRC_B}
	gcc ${COMMONSRC} ${UTILBASE} ${SRC_B} -o ./$@ ${INCDIR} ${LIBS}
./Consumer/consumer: ${UTILCONSUMER} ${COMMONHDR} ${SRC_C}
	gcc ${COMMONSRC} ${UTILCONSUMER} ${SRC_C} -o ./$@ ${INCDIR} ${LIBS}
./Producer/producer: ${UTILPRODUCER} ${COMMONHDR} ${SRC_P}
	gcc ${COMMONSRC} ${UTILPRODUCER} ${SRC_P} -o ./$@ ${INCDIR} ${LIBS}
