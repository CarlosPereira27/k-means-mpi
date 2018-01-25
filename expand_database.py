# -*- coding: utf-8 -*-
#!/usr/bin/python3

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

    def toString(self, index_clazz):
        """
        Representação em string da instância no padrão para salvar a base de dados
        no padrão csv.

        @param index_clazz : int
            índice da coluna em que a classe deve ficar
        """
        features_length = len(self.features)
        str_represent = ''
        if index_clazz == 0:
            str_represent += self.clazz
        else:
            str_represent += str(self.features[0])
            for i in range(1, index_clazz):
                str_represent += ',' + str(self.features[i])
            str_represent += ',' + self.clazz
        for i in range(index_clazz, features_length):
            str_represent += ',' + str(self.features[i])
        return str_represent + '\n'


class Database:
    """
    Representação de uma base de dados
    """

    def __init__(self, path_database, index_clazz, onlyint):
        """
        Construtor de uma base de dados.

        @param path_database : str
            caminho do arquivo que contém a base de dados em csv
        @param index_clazz : int
            índice da coluna em que a classe deve ficar
        @param onlyint : bool
            informa se as features das instâncias são formadas apenas
            por inteiros ou não
        """
        self.path_database = path_database
        self.instances = []
        self.index_clazz = index_clazz
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
            clazz = attrs[self.index_clazz]
            for i in range(self.index_clazz):
                if self.onlyint:
                    features.append(int(attrs[i]))
                else:
                    features.append(float(attrs[i]))
            for i in range(self.index_clazz + 1, N):
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
            print (instance.toString(self.index_clazz))
            
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
        smallest_features = [ sys.float_info.max ] * N
        for instance in self.instances:
            for i in range(N):
                if smallest_features[i] > instance.features[i]:
                    smallest_features[i] = instance.features[i]
        return smallest_features

    def getBiggestFeatures(self):
        """
        Encontra o maior valor de cada feature entre as instâncias desta
        base de dados.

        @return : list<float>
            lista com os maiores valores de cada feature
        """
        N = len(self.instances[0].features)
        biggest_features = [ 0 ] * N
        for instance in self.instances:
            for i in range(N):
                if biggest_features[i] < instance.features[i]:
                    biggest_features[i] = instance.features[i]
        return biggest_features

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

    def randomFloat(self, min_value, max_value):
        """
        Sorteia um número real aleatório entre min_value e max_value.

        @param min_value: float
            valor mínimo a ser gerado
        @param max_value: float
            valor máximo a ser gerado

        @return : float
            valor real gerado
        """
        return min_value + (random.random() * (max_value - min_value))

    def generateInstance(self, smallest_features, biggest_features, clazzes):
        """
        Gera uma instância aleatória. As features são geradas com valores no intervalo
        dos valores das features da base de dados.

        @param smallest_features: list<float>
            lista com os menores valores encontradas em cada feature
        @param biggest_feature: list<float>
            lista com os maiores valores encontradas em cada feature
        @param clazzes: list<str>
            lista de classes contidas na base de dados

        @return : Instance
            instância gerada
        """
        features_length = len(self.instances[0].features)
        features = [ 0 ] * features_length
        for i in range(features_length):
            if self.onlyint:
                features[i] = random.randint(smallest_features[i], biggest_features[i])
            else:
                features[i] = self.randomFloat(smallest_features[i], biggest_features[i])
        return Instance(self.randomClazz(clazzes), features)
        

    def generateBiggerDatabase(self, multiplier):
        """
        Gera uma base de dados $multiplier vezes maior que a atual de forma aleatória
        seguindo o padrão dessa base, limites de valores das features da base de dados.

        @param multiplier : int
            multiplicador do tamanho da base de dados
        """
        path_bigger_database = self.path_database + "_" + str(multiplier) + 'times'
        file = open(path_bigger_database,'w')

        smallest_features = self.getSmallestFeatures()
        biggest_features = self.getBiggestFeatures()
        clazzes = self.getClazzes()

        for instance in self.instances:
            file.write(instance.toString(self.index_clazz))
        
        qty_instances =  (multiplier - 1) * len(self.instances)
        print (qty_instances)
        for i in range(qty_instances):
            file.write(self.generateInstance(smallest_features, biggest_features, clazzes).toString(self.index_clazz))


        file.close()
        
def expandLetterDatabase():
    """
    Script de expansão da base de dados letter-recognition em 10 vezes.
    """
    path_database = "databases/letter-recognition/letter-recognition.data"
    index_clazz = 0
    multiplier = 10
    onlyint = True
    database = Database(path_database, index_clazz, onlyint) 
    database.generateBiggerDatabase(multiplier)

def expandSegmentationDatabase():
    """
    Script de expansão da base de dados segmentation em 10 vezes.
    """
    path_database = "databases/image/segmentation.data"
    index_clazz = 0
    multiplier = 10
    onlyint = False
    database = Database(path_database, index_clazz, onlyint) 
    database.generateBiggerDatabase(multiplier)


def main(argv):
    expandSegmentationDatabase()


if __name__ == "__main__":
    main(sys.argv[1:])