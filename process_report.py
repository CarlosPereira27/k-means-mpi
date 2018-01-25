# -*- coding: utf-8 -*-
#!/usr/bin/python3

from __future__ import unicode_literals
import sys, getopt
import os
import matplotlib.pyplot as plt
import matplotlib
import numpy as np

# --------------------------------------------------------
# ---------------- BEGIN Parâmetros ----------------------
# --------------------------------------------------------

# Valor crítico para um grau de confiança de 95%
critical_value_z = 1.96

# Arquivos de saída do relatório padrão
pathfiles = [ "reports_gcloud/letter-recognition/letter-recognition.csv", \
    "reports_gcloud/letter-recognition_10times/letter-recognition_10times.csv", \
    "reports_gcloud/segmentation/segmentation.csv", \
    "reports_gcloud/segmentation_10times/segmentation_10times.csv" ]

# Diretório do relatório
directory_report = "reports_gcloud/"

# Base de dados utilizadas
databases = [ "letter-recognition", "letter-recognition_10times", \
    "segmentation", "segmentation_10times" ]

# Quantidade de testes realizados
qty_tests = 10

# Sufixos dos tempos salvos
out_files_suffix = [ "time_in", "time_report", "time_kmeans", "time_total" ]


# --------------------------------------------------------
# ---------------- END Parâmetros ------------------------
# --------------------------------------------------------

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
    """
    Desenha um gráfico com quatro linhas para valores de y diferente e insere suas legendas.

    @param filename : str
        nome do arquivo em que o gráfico será salvo
    @param xdata : list
        dados do eixo x
    @param ydata : list
        dados do eixo y
    @param xlabel : str
        rótulo do eixo x
    @param ylabel : str
        rótulo do eixo y
    """
    plt.plot(xdata, ydata, 'bo-', lw = 3, mew = 5)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.savefig(os.path.join(filename + ".png"), dpi=300)
    plt.gcf().clear()

class Report:
    """
    Representa um relatório de uma determinada base de dados.
    """

    def __init__(self, qty_times, critical_value_z, qty_tests):
        """
        Constrói um relatório para uma determinada quantidade de tempos salvos, 
        determinado valor crítico do grau de confiança do intervalo de confiança,
        e determinada quantidade de testes.

        @param qty_times : int
            quantidade de tempos usados no relatório
        @param critical_value_z : float
            valor crítico do grau de confiança do intervalo de confiança
        @param qty_tests : int
            quantidade de testes realizados em cada cenário
        """
        self.qty_times = qty_times
        self.qty_tests = qty_tests
        self.critical_value_z = critical_value_z
        self.time_mean = list()
        self.dev_confid_inter = list()
        self.speedup = list()
        self.efficiency = list()
        for i in range(self.qty_times):
            self.time_mean.append(list())
            self.dev_confid_inter.append(list())
            self.speedup.append(list())
            self.efficiency.append(list())
        self.nprocs = list()


    def includeReportUnit(self, report_unit, index, nprocs_unit):
        """
        Inclui uma nova unidade de relatório nesse relatório.

        @param report_unit : list<list<int>>
            unidade de relatório, uma unidade de relatório é uma matrix onde cada
            linha corresponde a um tipo de tempo dos qty_times tipos de tempo e
            dentro de cada linha contém qty_times de tempos desse tipo
        @param index : int
            índice da unidade de relatório, representa o índice do número de processos
        @param nprocs_unit : int
            número de processos da unidade de relatório

        """
        self.calcMean(report_unit)
        self.calcDevConfidInter(report_unit, index)
        self.calcSpeedup(report_unit, index)
        self.calcEfficiency(report_unit, index, nprocs_unit)
        self.nprocs.append(nprocs_unit)

    def calcMean(self, report_unit):
        """
        Calcula a média dos tempos da unidade de relatório.

        @param report_unit : list<list<int>>
            unidade de relatório, uma unidade de relatório é uma matrix onde cada
            linha corresponde a um tipo de tempo dos qty_times tipos de tempo e
            dentro de cada linha contém qty_times de tempos desse tipo
        """
        for i in range(self.qty_times):
            self.time_mean[i].append(int(mean(report_unit[i])))

    def getStdDev(self, report_unit, index):
        """
        Calcula o desvio padrão dos tempos da unidade de relatório.

        @param report_unit : list<list<int>>
            unidade de relatório, uma unidade de relatório é uma matrix onde cada
            linha corresponde a um tipo de tempo dos qty_times tipos de tempo e
            dentro de cada linha contém qty_times de tempos desse tipo
        @param index : int
            índice da unidade de relatório, representa o índice do número de processos

        @return list<float>
            desvio padrão dos tempos da unidade de relatório.
        """
        std_deviation_report = list()
        for j in range(self.qty_times):
            std_deviation_report.append(standard_deviation(report_unit[j], self.time_mean[j][index]))
        return std_deviation_report

    def calcDevConfidInter(self, report_unit, index):
        """
        Calcula o intervalo de confiança da unidade de relatório.

        @param report_unit : list<list<int>>
            unidade de relatório, uma unidade de relatório é uma matrix onde cada
            linha corresponde a um tipo de tempo dos qty_times tipos de tempo e
            dentro de cada linha contém qty_times de tempos desse tipo
        @param index : int
            índice da unidade de relatório, representa o índice do número de processos
        """
        std_deviation_report = self.getStdDev(report_unit, index)
        for i in range(self.qty_times):
            self.dev_confid_inter[i].append(int(self.critical_value_z * \
                (std_deviation_report[i] / (pow(self.qty_tests, 0.5)))))

    def calcSpeedup(self, report_unit, index):
        """
        Calcula speedup da unidade de relatório.

        @param report_unit : list<list<int>>
            unidade de relatório, uma unidade de relatório é uma matrix onde cada
            linha corresponde a um tipo de tempo dos qty_times tipos de tempo e
            dentro de cada linha contém qty_times de tempos desse tipo
        @param index : int
            índice da unidade de relatório, representa o índice do número de processos
        """
        for i in range(self.qty_times):
            self.speedup[i].append(self.time_mean[i][0] / self.time_mean[i][index])

    def calcEfficiency(self, report_unit, index, nprocs_unit):
        """
        Calcula eficiência da unidade de relatório.

        @param report_unit : list<list<int>>
            unidade de relatório, uma unidade de relatório é uma matrix onde cada
            linha corresponde a um tipo de tempo dos qty_times tipos de tempo e
            dentro de cada linha contém qty_times de tempos desse tipo
        @param index : int
            índice da unidade de relatório, representa o índice do número de processos
        @param nprocs_unit : int
            número de processos da unidade de relatório
        """
        for i in range(self.qty_times):
            self.efficiency[i].append(self.speedup[i][index] / nprocs_unit)

    def generateFinalReportCsv(self, pathfile, out_files_suffix):
        """
        Gera as planilhas com os resultados estatísticos do relatório.

        @param pathfile : str
            caminho e nome do arquivo original do relatório
        @param out_files_suffix : list<str>
            lista de sufixos para planilhas de cada tipo de tempo analisado
        """
        for i in range(self.qty_times):
            file_out = open(pathfile[:-4] + "_" + out_files_suffix[i] + "_final.csv", 'w')
            file_out.write("nprocs,time_mean,confidence_interval,speedup,efficiency\n")
            N = len(self.nprocs)
            for j in range(N):
                file_out.write("%d" % (self.nprocs[j]))
                file_out.write(",%d,[%d - %d],%f,%f\n" % (self.time_mean[i][j], \
                    self.time_mean[i][j] - self.dev_confid_inter[i][j], \
                    self.time_mean[i][j] + self.dev_confid_inter[i][j], \
                    self.speedup[i][j], self.efficiency[i][j]))
            file_out.close()

    def generateCharts(self, pathfile, out_files_suffix):
        """
        Gera os gráficos com os resultados estatísticos do relatório.

        @param pathfile : str
            caminho e nome do arquivo original do relatório
        @param out_files_suffix : list<str>
            lista de sufixos para planilhas de cada tipo de tempo analisado
        """
        for i in range(self.qty_times):
            plotChart(pathfile[:-4] + "_" + out_files_suffix[i] + "_mean", self.nprocs, \
                self.time_mean[i], "nprocs", "tempo")
            plotChart(pathfile[:-4] + "_" + out_files_suffix[i] + "_speedup", self.nprocs, \
                self.speedup[i], "nprocs", "speedup")
            plotChart(pathfile[:-4] + "_" + out_files_suffix[i] + "_efficiency", \
                self.nprocs, self.efficiency[i], "nprocs", "eficiência")

def plotChartGeneral(filename, xdata, ydata0, leg0, ydata1, leg1, ydata2, leg2, \
        ydata3, leg3, xlabel, ylabel):
    """
    Desenha um gráfico com quatro linhas para valores de y diferente e insere suas legendas.

    @param filename : str
        nome do arquivo em que o gráfico será salvo
    @param xdata : list
        dados do eixo x
    @param ydata0 : list
        dados do eixo y, linha 0 
    @param leg0 : str
        legenda para a linha 0
    @param ydata1 : list
        dados do eixo y, linha 1 
    @param leg1 : str
        legenda para a linha 1
    @param ydata2 : list
        dados do eixo y, linha 2 
    @param leg2 : str
        legenda para a linha 2
    @param ydata3 : list
        dados do eixo y, linha 3 
    @param leg3 : str
        legenda para a linha 3
    @param xlabel : str
        rótulo do eixo x
    @param ylabel : str
        rótulo do eixo y
    """
    lw_val = 3
    mew_val = 5
    line0, = plt.plot(xdata, ydata0, 'bo-', lw = lw_val, mew = mew_val, label=leg0)
    line1, = plt.plot(xdata, ydata1, 'ro-', lw = lw_val, mew = mew_val, label=leg1)
    line2, = plt.plot(xdata, ydata2, 'yo-', lw = lw_val, mew = mew_val, label=leg2)
    line3, = plt.plot(xdata, ydata3, 'go-', lw = lw_val, mew = mew_val, label=leg3)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.legend(handles=[line0, line1, line2, line3]);
    plt.savefig(os.path.join(filename + ".png"), dpi=300)
    plt.gcf().clear()

def generateGeneralCharts(reports):
    """
    Gera os gráficos gerais do relatório.

    @param reports : list<Report>
        lista de relatórios de cada base de dados
    """
    qty_times = len(out_files_suffix)
    for i in range(qty_times):
        plotChartGeneral(directory_report + "report_" + out_files_suffix[i] + "_mean", \
            reports[0].nprocs, reports[0].time_mean[i], databases[0], \
            reports[1].time_mean[i], databases[1], \
            reports[2].time_mean[i], databases[2], \
            reports[3].time_mean[i], databases[3], \
            "nprocs", "tempo")
        plotChartGeneral(directory_report + "report_" + out_files_suffix[i] + "_speedup", \
            reports[0].nprocs, reports[0].speedup[i], databases[0], \
            reports[1].speedup[i], databases[1], \
            reports[2].speedup[i], databases[2], \
            reports[3].speedup[i], databases[3], \
            "nprocs", "speedup")
        plotChartGeneral(directory_report + "report_" + out_files_suffix[i] + "_efficiency", \
            reports[0].nprocs, reports[0].efficiency[i], databases[0], \
            reports[1].efficiency[i], databases[1], \
            reports[2].efficiency[i], databases[2], \
            reports[3].efficiency[i], databases[3], \
            "nprocs", "eficiência")

            
def main(argv):
    reports = list()
    qty_times = len(out_files_suffix)
    for pathfile in pathfiles:
        file = open(pathfile, 'r')
        reading = True
        report = Report(qty_times, critical_value_z, qty_tests)
        index = 0
        line = file.readline()
        while reading:
            report_unit = list()
            nprocs_aux = list()
            for j in range(qty_times):
                report_unit.append(list())

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
                for j in range(qty_times):
                    report_unit[j].append(int(tokens[j]))
                nprocs_aux.append(int(tokens[qty_times]))
            if not reading:
                break
            verifyNProcs(nprocs_aux)
            report.includeReportUnit(report_unit, index, nprocs_aux[0])
            index += 1

        file.close()
        #report.generateFinalReportCsv(pathfile, out_files_suffix)
        #report.generateCharts(pathfile, out_files_suffix)
        reports.append(report)
    generateGeneralCharts(reports)

if __name__ == "__main__":
    main(sys.argv[1:])