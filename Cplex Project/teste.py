inp = u"""S4M0,0,130,JOB1
S3M1,0,155,JOB3
S3M0,0,177,JOB2
S2M0,0,161,JOB0
S1M0,0,179,JOB4
S0M1,179,334,JOB4
S0M0,161,351,JOB0
S4M0,177,381,JOB2
S3M1,155,395,JOB1
S2M0,161,399,JOB3
S3M0,351,544,JOB0
S1M0,399,555,JOB3
S0M1,381,619,JOB2
S0M0,395,613,JOB1
S4M0,381,561,JOB4
S3M1,561,796,JOB4
S2M0,619,774,JOB2
S1M0,555,777,JOB0
S4M0,561,813,JOB3
S0M0,813,1012,JOB3
S2M0,774,969,JOB1
S1M0,777,1003,JOB2
S4M0,813,1047,JOB0
S2M0,969,1220,JOB4
S1M0,1003,1220,JOB1
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