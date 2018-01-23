import sys
import os.path
import random


class Instance:
    """
    Representa uma instância de uma base de dados.
    """

    def __init__(self, clazz, features):
        """
        Construtor da instância.

        @param clazz : str
            classe da instância    
        @param features : list<float>
            features da instância
        """
        self.clazz = clazz
        self.features = features

    def toString(self, indexClazz):
        """
        Representação em string da instância no padrão para salvar a base de dados
        no padrão csv.

        @param indexClazz : int
            índice da coluna em que a classe deve ficar
        """
        featuresLength = len(self.features)
        strRepresent = ''
        if indexClazz == 0:
            strRepresent += self.clazz
        else:
            strRepresent += str(self.features[0])
            for i in range(1, indexClazz):
                strRepresent += ',' + str(self.features[i])
            strRepresent += ',' + self.clazz
        for i in range(indexClazz, featuresLength):
            strRepresent += ',' + str(self.features[i])
        return strRepresent + '\n'


class Database:
    """
    Representação de uma base de dados
    """

    def __init__(self, path_database, indexClazz, onlyint):
        """
        Construtor de uma base de dados.

        @param path_database : str
            caminho do arquivo que contém a base de dados em csv
        @param indexClazz : int
            índice da coluna em que a classe deve ficar
        @param onlyint : bool
            informa se as features das instâncias são formadas apenas
            por inteiros ou não
        """
        self.path_database = path_database
        self.instances = []
        self.indexClazz = indexClazz
        self.onlyint = onlyint
        self.loadDatabase()
        
    def loadDatabase(self):
        """
        Carrega a base de dados do seu arquivo em csv
        """
        file = open(self.path_database, 'r')
        for line in file:
            if line[-1] == '\n':
                line = line[:-1]
            attrs = line.split(',')
            N = len(attrs)
            features = []
            clazz = attrs[self.indexClazz]
            for i in range(self.indexClazz):
                if self.onlyint:
                    features.append(int(attrs[i]))
                else:
                    features.append(float(attrs[i]))
            for i in range(self.indexClazz + 1, N):
                if self.onlyint:
                    features.append(int(attrs[i]))
                else:
                    features.append(float(attrs[i]))
            self.instances.append(Instance(clazz, features))
        file.close()

    def printDatabase(self):
        """
        Imprime a base de dados em csv
        """
        for instance in self.instances:
            print (instance.toString(self.indexClazz))
            
    def addInstance(self, instance):
        """
        Adiciona uma instância na base de dados

        @param instance : Instance
            instância a ser adicionada
        """
        self.instances.append(instance)

    def getSmallestFeatures(self):
        """
        Encontra o menor valor de cada feature entre as instâncias desta
        base de dados.

        @return : list<float>
            lista com os menores valores de cada feature
        """
        N = len(self.instances[0].features)
        smallestFeature = [ sys.float_info.max ] * N
        for instance in self.instances:
            for i in range(N):
                if smallestFeature[i] > instance.features[i]:
                    smallestFeature[i] = instance.features[i]
        return smallestFeature

    def getBiggestFeatures(self):
        """
        Encontra o maior valor de cada feature entre as instâncias desta
        base de dados.

        @return : list<float>
            lista com os maiores valores de cada feature
        """
        N = len(self.instances[0].features)
        biggestFeature = [ 0 ] * N
        for instance in self.instances:
            for i in range(N):
                if biggestFeature[i] < instance.features[i]:
                    biggestFeature[i] = instance.features[i]
        return biggestFeature

    def getClazzes(self):
        """
        Cria a lista de classes contidas na base de dados.

        @return : list<str>
            lista de classes contidas na base de dados
        """
        clazzes = set()
        for instance in self.instances:
            clazzes.add(instance.clazz)
        return list(clazzes)

    def randomClazz(self, clazzes):
        """
        Recupera uma classe aleatória de uma lista de classes.

        @param clazzes: list<str>
            lista de classes contidas na base de dados

        @return : str
            classe aleatória
        """
        return clazzes[random.randint(0, len(clazzes) - 1)]

    def randomFloat(self, minValue, maxValue):
        """
        Sorteia um número real aleatório entre minValue e maxValue.

        @param minValue: float
            valor mínimo a ser gerado
        @param maxValue: float
            valor máximo a ser gerado

        @return : float
            valor real gerado
        """
        return minValue + (random.random() * (maxValue - minValue))

    def generateInstance(self, smallestFeature, biggestFeature, clazzes):
        """
        Gera uma instância aleatória. As features são geradas com valores no intervalo
        dos valores das features da base de dados.

        @param smallestFeature: list<float>
            lista com os menores valores encontradas em cada feature
        @param biggestFeature: list<float>
            lista com os maiores valores encontradas em cada feature
        @param clazzes: list<str>
            lista de classes contidas na base de dados

        @return : Instance
            instância gerada
        """
        featuresLength = len(self.instances[0].features)
        features = [ 0 ] * featuresLength
        for i in range(featuresLength):
            if self.onlyint:
                features[i] = random.randint(smallestFeature[i], biggestFeature[i])
            else:
                features[i] = self.randomFloat(smallestFeature[i], biggestFeature[i])
        return Instance(self.randomClazz(clazzes), features)
        

    def generateBiggerDatabase(self, multiplier):
        """
        Gera uma base de dados $multiplier vezes maior que a atual de forma aleatória
        seguindo o padrão dessa base, limites de valores das features da base de dados.

        @param multiplier : int
            multiplicador do tamanho da base de dados
        """
        path_bigger_database = self.path_database + str(multiplier) + 'Times'
        file = open(path_bigger_database,'w')

        smallestFeature = self.getSmallestFeatures()
        biggestFeature = self.getBiggestFeatures()
        clazzes = self.getClazzes()

        for instance in self.instances:
            file.write(instance.toString(self.indexClazz))
        
        qtyInstances =  (multiplier - 1) * len(self.instances)
        print (qtyInstances)
        for i in range(qtyInstances):
            file.write(self.generateInstance(smallestFeature, biggestFeature, clazzes).toString(self.indexClazz))


        file.close()
        
def expandLetterDatabase():
    """
    Script de expansão da base de dados letter-recognition em 10 vezes.
    """
    path_database = "databases/letter-recognition/letter-recognition.data"
    indexClazz = 0
    multiplier = 10
    onlyint = True
    database = Database(path_database, indexClazz, onlyint) 
    database.generateBiggerDatabase(multiplier)

def expandSegmentationDatabase():
    """
    Script de expansão da base de dados segmentation em 10 vezes.
    """
    path_database = "databases/image/segmentation.data"
    indexClazz = 0
    multiplier = 10
    onlyint = False
    database = Database(path_database, indexClazz, onlyint) 
    database.generateBiggerDatabase(multiplier)


def main(argv):
    expandSegmentationDatabase()


if __name__ == "__main__":
    main(sys.argv[1:])