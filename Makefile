INCDIR=-I./Common
LIBS=-lGLESv2 -lEGL -lm -lX11 -lancillary 

COMMONSRC=./Common/esShader.c    \
          ./Common/esTransform.c \
          ./Common/esShapes.c

COMMONHRD=esUtil.h

UTILCONSUMER=./Consumer/esUtil.c
UTILPRODUCER=./Producer/esUtil.c

CH09SRC1=./Consumer/consumer.c
CH09SRCP=./Producer/producer.c

default: all

all:                           \
     ./Consumer/consumer       \
     ./Producer/producer

clean:
	rm ./Consumer/consumer ./Producer/producer

./Consumer/consumer: ${UTILCONSUMER} ${COMMONHDR} ${CH09SRC1}
	gcc ${COMMONSRC} ${UTILCONSUMER} ${CH09SRC1} -o ./$@ ${INCDIR} ${LIBS}
./Producer/producer: ${UTILPRODUCER} ${COMMONHDR} ${CH09SRCP}
	gcc ${COMMONSRC} ${UTILPRODUCER} ${CH09SRCP} -o ./$@ ${INCDIR} ${LIBS}
