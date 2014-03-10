lmds-objects = sompackexporter.o datamatrix.o exception.o calculatedistancematrix.o euclidean.o randomdatagenerator.o sompackloader.o dataset.o distancematrix.o commandlineargs.o dynamicdouble.o lmdscostfunction.o lmdsoptstrat.o ccaoptimization.o lmds.o distancematrixloader.o nervprobability.o goldensectionsearch.o inputprobentropy.o conjugategradientopt.o nervoptstrat.o recorder.o

nerv-objects = sompackexporter.o datamatrix.o exception.o calculatedistancematrix.o euclidean.o euclideansquared.o randomdatagenerator.o sompackloader.o dataset.o distancematrix.o commandlineargs.o dynamicdouble.o conjugategradientopt.o goldensectionsearch.o nervcostfunction.o nervoptstrat.o nervprobability.o inputprobentropy.o nerv.o distancematrixloader.o recorder.o

tcnerv-objects = sompackexporter.o datamatrix.o exception.o calculatedistancematrix.o euclidean.o euclideansquared.o randomdatagenerator.o sompackloader.o dataset.o distancematrix.o commandlineargs.o dynamicdouble.o conjugategradientopt.o goldensectionsearch.o tcnervcostfunction.o tcnervoptstrat.o nervprobability.o inputprobentropy.o tcnerv.o distancematrixloader.o recorder.o

geanerv-objects = sompackexporter.o datamatrix.o exception.o calculatedistancematrix.o euclidean.o euclideansquared.o randomdatagenerator.o sompackloader.o dataset.o distancematrix.o commandlineargs.o dynamicdouble.o conjugategradientopt.o goldensectionsearch.o geanervcostfunction.o geanervoptstrat.o nervprobability.o inputprobentropy.o geanerv.o distancematrixloader.o recorder.o

geanerv_nervinit-objects = sompackexporter.o datamatrix.o exception.o calculatedistancematrix.o euclidean.o euclideansquared.o randomdatagenerator.o sompackloader.o dataset.o distancematrix.o commandlineargs.o dynamicdouble.o conjugategradientopt.o goldensectionsearch.o nervcostfunction.o geanervcostfunction.o nervoptstrat.o geanervoptstrat.o nervprobability.o inputprobentropy.o  geanerv_nervinit.o distancematrixloader.o recorder.o

linearnerv-objects = sompackexporter.o datamatrix.o exception.o calculatedistancematrix.o euclidean.o euclideansquared.o randomdatagenerator.o sompackloader.o dataset.o distancematrix.o commandlineargs.o dynamicdouble.o conjugategradientopt.o goldensectionsearch.o nervcostfunction.o geanervcostfunction.o nervoptstrat.o geanervoptstrat.o nervprobability.o inputprobentropy.o  linearnerv.o distancematrixloader.o recorder.o

unisne-objects = sompackexporter.o datamatrix.o exception.o calculatedistancematrix.o euclidean.o euclideansquared.o randomdatagenerator.o sompackloader.o dataset.o distancematrix.o commandlineargs.o dynamicdouble.o conjugategradientopt.o goldensectionsearch.o unisnecostfunction.o unisneoptstrat.o nervprobability.o inputprobentropy.o unisne.o distancematrixloader.o recorder.o


gennerv-objects = sompackexporter.o datamatrix.o exception.o calculatedistancematrix.o euclidean.o euclideansquared.o randomdatagenerator.o sompackloader.o dataset.o distancematrix.o commandlineargs.o dynamicdouble.o gennervconjugategradientopt.o goldensectionsearch.o gennervcostfunction.o gennervoptstrat.o gennervexpectedneighbors.o gennerv.o distancematrixloader.o recorder.o

tnerv-objects = sompackexporter.o datamatrix.o exception.o calculatedistancematrix.o euclidean.o euclideansquared.o randomdatagenerator.o sompackloader.o dataset.o distancematrix.o commandlineargs.o dynamicdouble.o tnervconjugategradientopt.o goldensectionsearch.o tnervcostfunction.o tnervoptstrat.o tnervexpectedneighbors.o tnerv.o distancematrixloader.o recorder.o

coloropt-objects = sompackexporter.o datamatrix.o exception.o calculatedistancematrix.o euclidean.o euclideansquared.o randomdatagenerator.o sompackloader.o dataset.o distancematrix.o commandlineargs.o dynamicdouble.o conjugategradientopt.o goldensectionsearch.o nervcostfunction.o nervoptstrat.o nervprobability.o inputprobentropy.o coloroptcostfunction.o coloropt.o inversedynamicdouble.o 

measure-objects = conttrust.o datamatrix.o exception.o calculatedistancematrix.o euclidean.o sompackloader.o sompackexporter.o commandlineargs.o dataset.o distancematrix.o measuremain.o distancematrixloader.o

klmeasure-objects = datamatrix.o distancematrix.o calculatedistancematrix.o exception.o euclidean.o sompackloader.o commandlineargs.o distancematrixloader.o dataset.o nervprobability.o inputprobentropy.o euclideansquared.o goldensectionsearch.o

klrank-objects = datamatrix.o distancematrix.o calculatedistancematrix.o exception.o euclidean.o sompackloader.o commandlineargs.o distancematrixloader.o dataset.o nervprobability.o inputprobentropy.o euclideansquared.o goldensectionsearch.o rankmatrix.o

nervgradtest-objects = numericalgradientcomputer.o nervcostfunction.o nervprobability.o inputprobentropy.o goldensectionsearch.o sompackloader.o commandlineargs.o dynamicdouble.o distancematrix.o datamatrix.o exception.o nervgradienttest.o dataset.o euclidean.o euclideansquared.o calculatedistancematrix.o

pca-objects = sompackexporter.o datamatrix.o exception.o sompackloader.o dataset.o commandlineargs.o pcamain.o


#CXXFLAGS = -O3 -g -Wall -std=c++98 -pedantic

#CXXFLAGS = -g -O3 -Wall -std=c++98 -pedantic -march=pentium-m -msse3 -pipe -fomit-frame-pointer
CXXFLAGS = -O3 -g -Wall -std=c++98 -pedantic -I ./newmat 

.PHONY: all clean


all: lmds measure nerv gennerv klmeasure

coloropt : $(coloropt-objects)
	g++ $(CXXFLAGS) -o coloropt $(coloropt-objects)

newmat : 
	cd newmat; make -f nm_gnu.mak libnewmat.a; 

lmds : $(lmds-objects)
	g++ $(CXXFLAGS) -o lmds $(lmds-objects)

nerv : $(nerv-objects)
	g++ $(CXXFLAGS) -o nerv $(nerv-objects)

tcnerv : $(tcnerv-objects)
	g++ $(CXXFLAGS) -o tcnerv $(tcnerv-objects)

geanerv : $(geanerv-objects) newmat
	g++ $(CXXFLAGS) -lncurses -o geanerv $(geanerv-objects) newmat/libnewmat.a 

geanerv_nervinit : $(geanerv_nervinit-objects) newmat
	g++ $(CXXFLAGS) -lncurses -o geanerv_nervinit $(geanerv_nervinit-objects) newmat/libnewmat.a
	
linearnerv : $(linearnerv-objects) newmat
	g++ $(CXXFLAGS) -lncurses -o linearnerv $(linearnerv-objects) newmat/libnewmat.a 

unisne : $(unisne-objects)
	g++ $(CXXFLAGS) -lncurses -o unisne $(unisne-objects)

gennerv : $(gennerv-objects)
	g++ $(CXXFLAGS) -o gennerv $(gennerv-objects)

tnerv : $(tnerv-objects)
	g++ $(CXXFLAGS) -o tnerv $(tnerv-objects)

measure : $(measure-objects)
	g++ $(CXXFLAGS) -o measure $(measure-objects)

klmeasure : klmeasure.o $(klmeasure-objects)
	g++ $(CXXFLAGS) -o klmeasure klmeasure.o $(klmeasure-objects)

klrank : klrank.o $(klrank-objects)
	g++ $(CXXFLAGS) -o klrank klrank.o $(klrank-objects)


nervgradtest : $(nervgradtest-objects)
	g++ $(CXXFLAGS) -o nervgradtest $(nervgradtest-objects)

pca : $(pca-objects) newmat
	g++ $(CXXFLAGS) -o pca $(pca-objects) newmat/libnewmat.a

pcamain.o : pca.hh pcamain.cc sompackexporter.o datamatrix.o exception.o sompackloader.o dataset.o commandlineargs.o 
	g++ -c $(CXXFLAGS) pcamain.cc

conjugategradientopt.o : conjugategradientopt.hh optimizationstepbatch.hh optimizationstep.hh costfunction.hh conjugategradientopt.cc distancematrix.o datamatrix.o dataset.o recorder.o Makefile
	g++ -c $(CXXFLAGS) conjugategradientopt.cc

gennervconjugategradientopt.o : gennervconjugategradientopt.hh optimizationstepbatch.hh optimizationstep.hh costfunction.hh gennervconjugategradientopt.cc distancematrix.o datamatrix.o dataset.o recorder.o Makefile
	g++ -c $(CXXFLAGS) gennervconjugategradientopt.cc

tnervconjugategradientopt.o : tnervconjugategradientopt.hh optimizationstepbatch.hh optimizationstep.hh costfunction.hh tnervconjugategradientopt.cc distancematrix.o datamatrix.o dataset.o recorder.o Makefile
	g++ -c $(CXXFLAGS) tnervconjugategradientopt.cc

goldensectionsearch.o : goldensectionsearch.hh linesearch.hh goldensectionsearch.cc datamatrix.o Makefile
	g++ -c $(CXXFLAGS) goldensectionsearch.cc

nervcostfunction.o : nervcostfunction.hh costfunction.hh linesearch.hh nervcostfunction.cc distancematrix.hh dataset.hh dynamicdouble.hh euclideansquared.hh nervprobability.hh Makefile
	g++ -c $(CXXFLAGS) nervcostfunction.cc

tcnervcostfunction.o : tcnervcostfunction.hh costfunction.hh linesearch.hh tcnervcostfunction.cc distancematrix.hh dataset.hh dynamicdouble.hh euclideansquared.hh nervprobability.hh Makefile
	g++ -c $(CXXFLAGS) tcnervcostfunction.cc

geanervcostfunction.o : geanervcostfunction.hh costfunction.hh linesearch.hh geanervcostfunction.cc distancematrix.hh dataset.hh dynamicdouble.hh euclideansquared.hh nervprobability.hh costapprox.cc gradapprox.cc Makefile
	g++ -c $(CXXFLAGS) geanervcostfunction.cc

unisnecostfunction.o : unisnecostfunction.hh costfunction.hh linesearch.hh unisnecostfunction.cc distancematrix.hh dataset.hh dynamicdouble.hh euclideansquared.hh nervprobability.hh Makefile
	g++ -c $(CXXFLAGS) unisnecostfunction.cc

gennervcostfunction.o : gennervcostfunction.hh costfunction.hh linesearch.hh gennervcostfunction.cc distancematrix.hh dataset.hh dynamicdouble.hh euclideansquared.hh nervprobability.hh Makefile
	g++ -c $(CXXFLAGS) gennervcostfunction.cc

tnervcostfunction.o : tnervcostfunction.hh costfunction.hh linesearch.hh tnervcostfunction.cc distancematrix.hh dataset.hh dynamicdouble.hh euclideansquared.hh nervprobability.hh Makefile
	g++ -c $(CXXFLAGS) tnervcostfunction.cc

coloroptcostfunction.o : coloroptcostfunction.hh costfunction.hh coloroptcostfunction.cc dataset.hh dynamicdouble.hh Makefile
	g++ -c $(CXXFLAGS) coloroptcostfunction.cc

nervoptstrat.o : nervoptstrat.hh optimizationstrategy.hh optimizationstep.hh costfunction.hh nervoptstrat.cc dataset.o sompackexporter.o Makefile
	g++ -c $(CXXFLAGS) nervoptstrat.cc

tcnervoptstrat.o : tcnervoptstrat.hh optimizationstrategy.hh optimizationstep.hh costfunction.hh tcnervoptstrat.cc dataset.o sompackexporter.o Makefile
	g++ -c $(CXXFLAGS) tcnervoptstrat.cc

geanervoptstrat.o : geanervoptstrat.hh optimizationstrategy.hh optimizationstep.hh costfunction.hh geanervoptstrat.cc dataset.o sompackexporter.o Makefile
	g++ -c $(CXXFLAGS) geanervoptstrat.cc

unisneoptstrat.o : unisneoptstrat.hh optimizationstrategy.hh optimizationstep.hh costfunction.hh unisneoptstrat.cc dataset.o sompackexporter.o Makefile
	g++ -c $(CXXFLAGS) unisneoptstrat.cc

gennervoptstrat.o : gennervoptstrat.hh optimizationstrategy.hh optimizationstep.hh costfunction.hh gennervoptstrat.cc dataset.o sompackexporter.o Makefile
	g++ -c $(CXXFLAGS) gennervoptstrat.cc

tnervoptstrat.o : tnervoptstrat.hh optimizationstrategy.hh optimizationstep.hh costfunction.hh tnervoptstrat.cc dataset.o sompackexporter.o Makefile
	g++ -c $(CXXFLAGS) tnervoptstrat.cc

conjgradoptstrat.o : conjgradoptstrat.hh optimizationstrategy.hh optimizationstep.hh costfunction.hh conjgradoptstrat.cc dataset.o Makefile
	g++ -c $(CXXFLAGS) conjgradoptstrat.cc

euclideansquared.o : euclideansquared.hh metric.hh euclideansquared.cc datamatrix.o Makefile
	g++ -c $(CXXFLAGS) euclideansquared.cc

exception.o : exception.cc exception.hh Makefile
	g++ -c $(CXXFLAGS) exception.cc

datamatrix.o : datamatrix.hh datamatrix.cc exception.hh Makefile
	g++ -c $(CXXFLAGS) datamatrix.cc

dynamicdouble.o : dynamicdouble.hh dynamicdouble.cc Makefile
	g++ -c $(CXXFLAGS) dynamicdouble.cc

inversedynamicdouble.o : dynamicdouble.hh inversedynamicdouble.hh inversedynamicdouble.cc Makefile
	g++ -c $(CXXFLAGS) inversedynamicdouble.cc

dataset.o : dataset.cc dataset.hh datamatrix.hh Makefile
	g++ -c $(CXXFLAGS) dataset.cc

calculatedistancematrix.o : calculatedistancematrix.cc calculatedistancematrix.hh datamatrix.o dataset.o Makefile
	g++ -c $(CXXFLAGS) calculatedistancematrix.cc

distancematrix.o : distancematrix.cc distancematrix.hh calculatedistancematrix.hh datamatrix.o dataset.o euclidean.o Makefile
	g++ -c $(CXXFLAGS) distancematrix.cc

euclidean.o : euclidean.cc euclidean.hh metric.hh dataset.o datamatrix.o Makefile
	g++ -c $(CXXFLAGS) euclidean.cc

sompackloader.o : dataloader.hh sompackloader.cc sompackloader.hh dataset.o datamatrix.o Makefile
	g++ -c $(CXXFLAGS) sompackloader.cc

sompackexporter.o : dataexporter.hh sompackexporter.cc sompackexporter.hh dataset.o exception.o datamatrix.o  Makefile
	g++ -c $(CXXFLAGS) sompackexporter.cc

randomdatagenerator.o : dataloader.hh randomdatagenerator.cc randomdatagenerator.hh dataset.o datamatrix.o  Makefile
	g++ -c $(CXXFLAGS) randomdatagenerator.cc

commandlineargs.o : commandlineargs.cc commandlineargs.hh exception.o Makefile
	g++ -c $(CXXFLAGS) commandlineargs.cc

lmds.o : sompackloader.hh dataset.o distancematrix.o sompackexporter.o sompackloader.o randomdatagenerator.o euclidean.o commandlineargs.o lmdscostfunction.o ccaoptimization.o lmdsoptstrat.o lmds.cc  Makefile
	g++ -c $(CXXFLAGS) lmds.cc

nerv.o : sompackloader.o dataset.o distancematrix.o datamatrix.o sompackexporter.o sompackloader.o randomdatagenerator.o euclidean.o commandlineargs.o nervcostfunction.o nervoptstrat.o conjugategradientopt.o nerv.cc Makefile
	g++ -c $(CXXFLAGS) nerv.cc

tcnerv.o : sompackloader.o dataset.o distancematrix.o datamatrix.o sompackexporter.o sompackloader.o randomdatagenerator.o euclidean.o commandlineargs.o tcnervcostfunction.o tcnervoptstrat.o conjugategradientopt.o tcnerv.cc Makefile
	g++ -c $(CXXFLAGS) tcnerv.cc

geanerv.o : sompackloader.o dataset.o distancematrix.o datamatrix.o sompackexporter.o sompackloader.o randomdatagenerator.o euclidean.o commandlineargs.o geanervcostfunction.o geanervoptstrat.o conjugategradientopt.o geanerv.cc pca.hh clustersandexacts.c Makefile
	g++ -c $(CXXFLAGS) geanerv.cc

geanerv_nervinit.o : sompackloader.o dataset.o distancematrix.o datamatrix.o sompackexporter.o sompackloader.o randomdatagenerator.o euclidean.o commandlineargs.o geanervcostfunction.o nervcostfunction.o geanervoptstrat.o nervoptstrat.o conjugategradientopt.o geanerv_nervinit.cc pca.hh clustersandexacts.c Makefile
	g++ -c $(CXXFLAGS) geanerv_nervinit.cc
	
linearnerv.o : sompackloader.o dataset.o distancematrix.o datamatrix.o sompackexporter.o sompackloader.o randomdatagenerator.o euclidean.o commandlineargs.o geanervcostfunction.o nervcostfunction.o geanervoptstrat.o nervoptstrat.o conjugategradientopt.o linearnerv.cc pca.hh clustersandexacts.c Makefile
	g++ -c $(CXXFLAGS) linearnerv.cc

unisne.o : sompackloader.o dataset.o distancematrix.o datamatrix.o sompackexporter.o sompackloader.o randomdatagenerator.o euclidean.o commandlineargs.o unisnecostfunction.o unisneoptstrat.o conjugategradientopt.o unisne.cc Makefile
	g++ -c $(CXXFLAGS) unisne.cc

gennerv.o : sompackloader.o dataset.o distancematrix.o datamatrix.o sompackexporter.o sompackloader.o randomdatagenerator.o euclidean.o commandlineargs.o gennervcostfunction.o gennervoptstrat.o gennervconjugategradientopt.o gennerv.cc Makefile
	g++ -c $(CXXFLAGS) gennerv.cc

tnerv.o : sompackloader.o dataset.o distancematrix.o datamatrix.o sompackexporter.o sompackloader.o randomdatagenerator.o euclidean.o commandlineargs.o tnervcostfunction.o tnervoptstrat.o tnervconjugategradientopt.o tnerv.cc Makefile
	g++ -c $(CXXFLAGS) tnerv.cc

coloropt.o : coloropt.cc sompackloader.o dataset.o distancematrix.o datamatrix.o sompackexporter.o sompackloader.o randomdatagenerator.o euclidean.o commandlineargs.o nervcostfunction.o nervoptstrat.o conjugategradientopt.o coloroptcostfunction.o Makefile 
	g++ -c $(CXXFLAGS) coloropt.cc

coloropt.o : coloropt.cc sompackloader.o dataset.o distancematrix.o datamatrix.o sompackexporter.o sompackloader.o randomdatagenerator.o euclidean.o commandlineargs.o nervcostfunction.o nervoptstrat.o conjugategradientopt.o coloroptcostfunction.o Makefile 
	g++ -c $(CXXFLAGS) coloropt.cc

conttrust.o : conttrust.cc conttrust.hh datamatrix.o distancematrix.o dataset.o exception.o  distcomp.hh Makefile
	g++ -c $(CXXFLAGS) conttrust.cc


measuremain.o : measuremain.cc sompackloader.hh dataset.o distancematrix.o euclidean.o randomdatagenerator.o conttrust.o datamatrix.o commandlineargs.o Makefile
	g++ -c $(CXXFLAGS) measuremain.cc

ccaoptimization.o : optimizationstep.hh ccaoptimization.cc ccaoptimization.hh distancematrix.o dataset.o euclidean.o Makefile
	g++ -c $(CXXFLAGS) ccaoptimization.cc

lmdscostfunction.o : lmdscostfunction.cc lmdscostfunction.hh separablecostfunction.hh costfunction.hh dynamicdouble.o distancematrix.o dataset.o Makefile
	g++ -c $(CXXFLAGS) lmdscostfunction.cc

lmdsoptstrat.o : optimizationstrategy.hh costfunction.hh optimizationstep.hh lmdsoptstrat.cc lmdsoptstrat.hh dataset.o distancematrix.o Makefile
	g++ -c $(CXXFLAGS) lmdsoptstrat.cc

numericalgradientcomputer.o : numericalgradientcomputer.hh numericalgradientcomputer.cc costfunction.hh datamatrix.o Makefile
	g++ -c $(CXXFLAGS) numericalgradientcomputer.cc

nervgradienttest.o : nervgradienttest.cc nervcostfunction.hh numericalgradientcomputer.hh sompackloader.o dynamicdouble.o commandlineargs.o goldensectionsearch.o conjugategradientopt.o datamatrix.o Makefile
	g++ -c $(CXXFLAGS) nervgradienttest.cc

gradtest.o : gradtest.hh gradtest.cc costfunction.hh datamatrix.o Makefile
	g++ -c $(CXXFLAGS) gradtest.cc


simplecostfunction.o : simplecostfunction.hh simplecostfunction.cc costfunction.hh datamatrix.o Makefile
	g++ -c $(CXXFLAGS) simplecostfunction.cc

testnumgrad.o : simplecostfunction.o numericalgradientcomputer.o sompackloader.o dataset.o commandlineargs.o testnumgrad.cc Makefile
	g++ -c $(CXXFLAGS) testnumgrad.cc

conjgradtest.o : simplecostfunction.o sompackloader.o goldensectionsearch.o commandlineargs.o dataset.o conjugategradientopt.o conjgradtest.cc Makefile
	g++ -c $(CXXFLAGS) conjgradtest.cc

nervprobability.o : nervprobability.hh nervprobability.cc calculatedistancematrix.o distancematrix.o goldensectionsearch.o Makefile
	g++ -c $(CXXFLAGS) nervprobability.cc

tcnervprobability.o : nervprobability.hh tcnervprobability.cc calculatedistancematrix.o distancematrix.o goldensectionsearch.o Makefile
	g++ -c $(CXXFLAGS) nervprobability.cc

inputprobentropy.o : inputprobentropy.hh inputprobentropy.cc costfunction.hh nervprobability.o Makefile
	g++ -c $(CXXFLAGS) inputprobentropy.cc

gennervexpectedneighbors.o : gennervexpectedneighbors.hh gennervexpectedneighbors.cc costfunction.hh nervprobability.o Makefile
	g++ -c $(CXXFLAGS) gennervexpectedneighbors.cc

tnervexpectedneighbors.o : tnervexpectedneighbors.hh tnervexpectedneighbors.cc costfunction.hh nervprobability.o Makefile
	g++ -c $(CXXFLAGS) tnervexpectedneighbors.cc

distancematrixloader.o : distancematrix.o distancematrixloader.hh distancematrixloader.cc Makefile
	g++ -c $(CXXFLAGS) distancematrixloader.cc

klmeasure.o : $(klmeasure-objects) klmeasure.cc
	g++ -c $(CXXFLAGS) klmeasure.cc

klrank.o : $(klrank-objects) klrank.cc
	g++ -c $(CXXFLAGS) klrank.cc

rankmatrix.o : rankmatrix.hh rankmatrix.cc distancematrix.o distcomp.hh
	g++ -c $(CXXFLAGS) rankmatrix.cc

recorder.o : recorder.hh recorder.cc sompackexporter.o datamatrix.o
	g++ -c $(CXXFLAGS) recorder.cc

clean :
	-rm lmds nerv measure fnerv klmeasure *.o 
