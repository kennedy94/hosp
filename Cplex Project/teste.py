inp = u"""S4M0,0,180,JOB4
S3M1,0,155,JOB3
S3M0,0,177,JOB2
S2M0,0,195,JOB1
S1M0,0,222,JOB0
S0M1,155,354,JOB3
S0M0,177,415,JOB2
S3M1,180,415,JOB4
S3M0,195,435,JOB1
S4M0,222,456,JOB0
S2M0,354,592,JOB3
S1M0,415,594,JOB4
S2M0,592,753,JOB0
S4M0,456,660,JOB2
S0M1,594,749,JOB4
S3M1,753,946,JOB0
S0M0,435,653,JOB1
S1M0,594,750,JOB3
S0M0,946,1136,JOB0
S4M0,660,790,JOB1
S1M0,750,976,JOB2
S4M0,790,1042,JOB3
S2M0,753,1004,JOB4
S1M0,976,1193,JOB1
S2M0,1004,1159,JOB2
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