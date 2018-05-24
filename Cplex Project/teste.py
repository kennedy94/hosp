inp = u"""S0M0,5.81668e-12,30.9,JOB0
S0M0,52.9,78.3,JOB1
S0M0,31.9,52,JOB2
S0M0,78.4,102,JOB3
S1M0,65.6,101,JOB0
S1M0,17.2,52.9,JOB1
S1M0,55.6,65.6,JOB2
S1M0,0,17.2,JOB3
S2M0,101,102,JOB0
S2M0,78.3,98,JOB1
S2M0,0,31.9,JOB2
S2M0,31.9,78.3,JOB3
S3M0,30.9,61.6,JOB0
S3M0,0,14.5,JOB1
S3M0,68.6,102,JOB2
S3M0,17.2,28.6,JOB3
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