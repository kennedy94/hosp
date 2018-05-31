inp = u"""S4M0,0,180,JOB4
S3M1,180,415,JOB4
S2M0,415,666,JOB4
S1M0,666,845,JOB4
S0M1,845,1000,JOB4
S4M0,180,432,JOB3
S3M0,432,587,JOB3
S2M0,666,904,JOB3
S1M0,904,1060,JOB3
S0M0,1060,1259,JOB3
S4M0,432,636,JOB2
S3M1,636,813,JOB2
S2M0,904,1059,JOB2
S1M0,1060,1286,JOB2
S0M1,1286,1524,JOB2
S4M0,636,766,JOB1
S3M0,766,1006,JOB1
S2M0,1059,1254,JOB1
S1M0,1286,1503,JOB1
S0M0,1503,1721,JOB1
S4M0,766,1000,JOB0
S3M1,1000,1193,JOB0
S2M0,1254,1415,JOB0
S1M0,1503,1725,JOB0
S0M1,1725,1915,JOB0
"""

import pandas as pd
import io
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.font_manager import FontProperties



df = pd.read_csv(io.StringIO(inp), header=None, names=["Task", "Start", "Finish", "Resource"] )
df["Diff"] = df.Finish - df.Start


color = {"JOB0":"black", "JOB1":"gray", "JOB2":"green" , "JOB3":"blue", "JOB4":"red"}


blackpatch	= mpatches.Patch(color='black', label='JOB1')
graypatch	= mpatches.Patch(color='gray', label='JOB2')
greenpatch	= mpatches.Patch(color='green', label='JOB3')
bluepatch	= mpatches.Patch(color='blue', label='JOB4')
red_patch	= mpatches.Patch(color='red', label='JOB5')

fig,ax=plt.subplots(figsize=(6,3))


labels=[]


for i, task in enumerate(df.groupby("Task")):
	labels.append(task[0])
	for r in task[1].groupby("Resource"):
		data = r[1][["Start", "Diff"]]
		ax.broken_barh(data.values, (i-0.4,0.8), color=color[r[0]] ) #label=r[0])



ax.set_yticks(range(len(labels)))
ax.set_yticklabels(labels) 
ax.set_xlabel("Time")

fontP = FontProperties()
fontP.set_size('small')
plt.legend(handles=[blackpatch,graypatch,greenpatch,bluepatch, red_patch],bbox_to_anchor=(1.05, 1), loc=2, borderaxespad=0.)

plt.tight_layout()       
#plt.show()
plt.savefig('grafico.png', bbox_inches='tight')