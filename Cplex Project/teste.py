inp = u"""S1M0,0,226,JOB2
S0M1,0,218,JOB1
S2M0,0,238,JOB3
S3M1,226,403,JOB2
S1M0,226,443,JOB1
S0M0,0,190,JOB0
S0M0,238,437,JOB3
S4M0,0,180,JOB4
S3M0,443,683,JOB1
S1M0,443,599,JOB3
S2M0,238,489,JOB4
S4M0,190,424,JOB0
S2M0,489,650,JOB0
S4M0,424,628,JOB2
S3M1,650,843,JOB0
S2M0,683,878,JOB1
S0M1,489,644,JOB4
S1M0,644,823,JOB4
S4M0,628,880,JOB3
S0M0,628,866,JOB2
S1M0,843,1065,JOB0
S4M0,880,1010,JOB1
S3M0,823,1058,JOB4
S3M1,880,1035,JOB3
S2M0,878,1033,JOB2
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