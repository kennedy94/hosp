inp = u"""S0M0,92,150,JOB0
S0M0,223,287,JOB1
S0M0,175,223,JOB2
S0M0,150,175,JOB3
S0M0,27,58,JOB4
S1M0,167,192,JOB0
S1M0,108,167,JOB1
S1M0,19,108,JOB2
S1M0,192,287,JOB3
S1M0,0,19,JOB4
S2M0,2,5,JOB0
S2M0,37,108,JOB1
S2M0,5,17,JOB2
S2M0,0,2,JOB3
S2M0,19,27,JOB4
S3M0,9,14,JOB0
S3M0,14,37,JOB1
S3M0,235,287,JOB2
S3M0,37,115,JOB3
S3M0,187,235,JOB4
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