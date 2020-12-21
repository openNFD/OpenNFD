import matplotlib
#matplotlib.use('Agg')
import matplotlib.pyplot as plt
#from matplotlib.backends.backend_pdf import PdfPages
#from pylab import *
#import matplotlib.gridspec as gridspec
import numpy as np
import re

path_ratelimiter = "./rate_limiter.txt"
path_tput = "./rate_limiter_throughput.pdf"
path_pps = "./rate_limiter_pps.pdf"
def getData(path):
    data    = []
    results = []
    with open(path,'r') as f:
        flag = True
        index_rate = 6
        index_size = 7
        index_tput = 5
        for line in f.readlines():
            aList = line.split(",")

            rate  = float(aList[index_rate].strip().split()[2])/1000000           
            size  = float(aList[index_size].strip().split()[2])
            Tput  = float(aList[index_tput].strip().split()[2])/1000000
            results.append((int(rate),size,int(Tput)))

    results= sorted(sorted(results,key=lambda x: x[0]),key=lambda x:x[1])
    last = results[0]
    count=1
    su=last[2]
    for item in results[1:]:
        if item[0]==last[0] and item[1] == last[1]:
            count+=1
            su+=item[2]
            continue
        data.append((last[0],last[1],su/count))
        count=1
        su=item[2]
        last=item
    data.append((last[0],last[1],su/count))
    return data 

def plotRateLimiter():

    #colors = ["#66ccff","#CD5555","#90EE90","#CDCD00","#A1A1A1","#CD00CD"]
    colors = ['#0099cc','#cccccc','#ff6666','#ffcccc','#ffff99','#ccccff']
    #colors = ["#FFC8B4","#FFA488","#FF7744","#FF5511","#E63F00","#C63300"]
    name_x     = []
    tput_rt = []
    pps_rt = []

    data_rt = getData(path_ratelimiter)
    data_rt = sorted(data_rt,key=lambda x: x[1])

    to_left = 0.08
    width = 0.14 
    x=np.array([0.0,1.0,2.0,3.0,4.0,5.0]) 

    plt.figure(figsize=(6,4.5))
    ax = plt.subplot(1,1,1)
    #matplotlib.backend_bases.GraphicsContextBase.set_hatch_color = 'b'
        
    offset=[to_left+ (1-2.0*to_left)/6.0*(0.5+i) for i in range(0,6)]
    labels = [i+' Bytes' for i in ['64','128','256','512','1024','1500']]
    hatches= ['//', 'x', '\\', '/', '-', '\\\\']
    
    # print data_rt[-1][2]
    for flag in range(0,6):
        ax.bar(x+offset[flag]*np.ones([1,6])[0], [ data_rt[i][2] for i in range(flag*6,(flag+1)*6) ], width, color =colors[flag], label=labels[flag], hatch = hatches[flag], edgecolor="black", linewidth=1)
        
    ax.set_xlabel("Rate (mbps)",fontsize= 17)
    ax.set_ylabel("Throughput (mbps)", fontsize=17)
    ax.set_xlim(0,6)
    ax.set_ylim(0, 1300)
    ax.yaxis.grid(True,linestyle='--')
        
    leg = plt.legend(loc="upper left",ncol=2, handleheight=1.3, handlelength=1.5, shadow=False,numpoints=1) 
    for t in leg.get_texts(): 
        t.set_fontsize(13) 
    plt.xticks(x + 0.5, ['100','200','400','600','800','1000'] ,rotation=0, fontsize=15)
    plt.yticks(fontsize=15)
    plt.tight_layout()
    plt.savefig(path_tput)
    # plt.show() 
        
        

    plt.figure(figsize=(6,4.5))
    ax = plt.subplot(1,1,1)
    #matplotlib.backend_bases.GraphicsContextBase.set_hatch_color = 'b'
        
    offset=[to_left+ (1-2.0*to_left)/6.0*(0.5+i) for i in range(0,6)]
    labels = [i+' Bytes' for i in ['64','128','256','512','1024','1500']]
       
    for flag in range(0,6):
        ax.bar(x+offset[flag]*np.ones([1,6])[0], [ data_rt[i][2]/8.0/data_rt[i][1]*1000 for i in range(flag*6,(flag+1)*6) ], width, color =colors[flag], label=labels[flag], hatch = hatches[flag], edgecolor="black", linewidth =1)
        
    ax.set_xlabel("Rate (mbps)",fontsize=17)
    ax.set_ylabel("Processing Rate (kpps)",fontsize=17)
    ax.set_xlim(0,6)
    ax.set_ylim(0,2300)
    ax.set_yticks([500,1000,1500,2000])
    ax.yaxis.grid(True,linestyle='--')
        
    leg = plt.legend(loc="upper left",ncol=2, handleheight=1.3, handlelength=1.5, shadow=False,numpoints=1) 
    for t in leg.get_texts(): 
        t.set_fontsize(13) 
       
    plt.xticks(x + 0.5, ['100','200','400','600','800','1000'] ,rotation=0, fontsize=15)
    plt.yticks(fontsize=15)

    plt.tight_layout()
    plt.savefig(path_pps)
    # plt.show() 
        

if __name__ == '__main__':

    plotRateLimiter()
