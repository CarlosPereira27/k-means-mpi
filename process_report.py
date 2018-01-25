# -*- coding: utf-8 -*-
#!/usr/bin/python3

from __future__ import unicode_literals
import sys, getopt
import os
import matplotlib.pyplot as plt
import matplotlib
import numpy as np

# Valor crítico para um grau de confiança de 95%
critical_value_z = 1.96

# Arquivos de saída do relatório padrão
pathfiles = [ "reports_gcloud/letter-recognition.csv", \
"reports_gcloud/letter-recognition_10times.csv", \
"reports_gcloud/segmentation.csv", \
"reports_gcloud/segmentation_10times.csv" ]
# Quantidade de testes realizados
qty_tests = 10
out_files_suffix = [ "time_in", "time_report", "time_kmeans", "time_total" ]
qty_times = 4

def mean(values):
    """
    Cálcula a média de uma coleção de valores.

    @param values : list<float>
        lista de valores a ser cálculada a média

    @return : float
        média da coleção de valores
    """
    return sum(values) / len(values)

def standard_deviation(values, mean):
    """
    Cálcula o desvio padrão de uma coleção de valores.

    @param values : list<float>
        lista de valores a ser cálculado o desvio padrão
    
    @param mean : float
        média dos valores analisados

    @return : float
        desvio padrão da coleção de valores
    """
    std_deviation = 0
    for value in values:
        std_deviation += pow(value - mean, 2)
    return pow(std_deviation / (len(values) - 1), 0.5)

def verifyNProcs(nprocs):
    """
    Valida se todos números de processos dos testes do cenário analisado
    estão iguais (corretos), caso esteja incorreto a aplicação encerra
    com erro

    @param nprocs : int
        número de processos dos testes do cenário analisado
    """
    for i in range(qty_tests - 1):
        if nprocs[i] != nprocs[i + 1]:
            print ("ERRO! Caso de teste com número de processos diferentes.")
            print (nprocs)
            exit(2)

def plotChart(filename, xdata, ydata, xlabel, ylabel):
    plt.plot(xdata, ydata, 'bo-', lw = 3, mew = 7)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.savefig(os.path.join(filename + ".png"), dpi=300)
    plt.gcf().clear()

def main(argv):
    for pathfile in pathfiles:
        file = open(pathfile, 'r')
        reading = True
        mean_report = list()
        dev_confid_inter = list()
        speedup = list()
        efficiency = list()
        for j in range(qty_times):
            mean_report.append(list())
            dev_confid_inter.append(list())
            speedup.append(list())
            efficiency.append(list())
        nprocs = list()
        index = 0
        line = file.readline()
        while reading:
            report = list()
            nprocs_aux = list()
            for j in range(qty_times):
                report.append(list())

            for i in range(qty_tests):
                line = file.readline()
                if not line:
                    # Se não consegir ler no ínicio de um cenário de teste, arquivo
                    # acabou corretamente, caso contrário não, então a aplicação 
                    # encerra com erro
                    if i == 0:
                        reading = False
                        break
                    print ("ERRO! Não foi possível ler cenário de teste inteiro.")
                    exit(2)

                tokens = line.split(",")
                # print(tokens)
                for j in range(qty_times):
                    report[j].append(int(tokens[j]))
                nprocs_aux.append(int(tokens[qty_times]))
            if not reading:
                break
            verifyNProcs(nprocs_aux)

            # Calcula a média dos tempos do relatório
            for j in range(qty_times):
                mean_report[j].append(int(mean(report[j])))

            # Calcula o desvio padrão dos tempos do relatório
            std_deviation_report = list()
            for j in range(qty_times):
                std_deviation_report.append(standard_deviation(report[j], mean_report[j][index]))
            
            # Calcula o intervalo de confiança
            for j in range(qty_times):
                dev_confid_inter[j].append(int(critical_value_z * \
                    (std_deviation_report[j] / (pow(qty_tests, 0.5)))))

            # Calcula speedup
            for j in range(qty_times):
                speedup[j].append(mean_report[j][0] / mean_report[j][index])
            
            # Calcula eficiência
            for j in range(qty_times):
                efficiency[j].append(speedup[j][index] / nprocs_aux[0])
            
            nprocs.append(nprocs_aux[0])
            index += 1

        file.close()

        for i in range(qty_times):
            file_out = open(pathfile[:-4] + "_" + out_files_suffix[i] + "_final.csv", 'w')
            file_out.write("nprocs,mean,confidence_interval,speedup,efficiency\n")
            N = len(nprocs)
            for j in range(N):
                file_out.write("%d" % (nprocs[j]))
                file_out.write(",%d,[%d - %d],%f,%f\n" % (mean_report[i][j], \
                    mean_report[i][j] - dev_confid_inter[i][j], mean_report[i][j] + \
                    dev_confid_inter[i][j], speedup[i][j], efficiency[i][j]))
            file_out.close()

        for i in range(qty_times):
            plotChart(pathfile[:-4] + "_" + out_files_suffix[i] + "_mean", nprocs, mean_report[i], \
                "nprocs", "tempo")
            plotChart(pathfile[:-4] + "_" + out_files_suffix[i] + "_speedup", nprocs, \
                speedup[i], "nprocs", "speedup")
            plotChart(pathfile[:-4] + "_" + out_files_suffix[i] + "_efficiency", \
                nprocs, efficiency[i], "nprocs", "eficiência")


if __name__ == "__main__":
    main(sys.argv[1:])