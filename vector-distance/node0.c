#include <stdio.h>

extern struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };

extern int TRACE;
extern int YES;
extern int NO;
extern float clocktime;

struct distance_table 
{
  int costs[4][4];
} dt0;



/* students to write the following two routines, and maybe some others */

extern void tolayer2(struct rtpkt packet);
void printdt0(struct distance_table *dtptr);

int min0(int a,int b)
{
  if(a>=b)
  {
    return b;
  }
  else
  {
    return a;
  }
  return 0;
}

void send0(int arr[])
{
  struct rtpkt sendpkt;

  sendpkt.sourceid=0;
  for(int i=0;i<4;i++)
  {
    int count=9999;
    for(int j=0;j<4;j++)
    {
      count=min0(count,dt0.costs[i][j]);
    }
    sendpkt.mincost[i]=count;
  }
  printf("Node 0's current mincost: [%d, %d, %d, %d]\n",
       sendpkt.mincost[0], sendpkt.mincost[1], sendpkt.mincost[2], sendpkt.mincost[3]);
  
  if(arr!=NULL)
  {
    int change=0;
    for(int m=0;m<4;m++)
    {
      if(arr[m]!=sendpkt.mincost[m])
      {
        change=1;
      }
    }
    if(change==0)
    {
      printf("Distance table updated: no\n");
      return;
    }
  }
  sendpkt.destid=1;
  tolayer2(sendpkt);
  sendpkt.destid=2;
  tolayer2(sendpkt);
  sendpkt.destid=3;
  tolayer2(sendpkt);
  
  if(arr!=NULL)
  {
    printf("Distance table updated: yes\n");
  }
   printf("Packets sent to: 1,2,3\n\n");

}

void rtinit0() 
{
  for(int i=0;i<4;i++)
  {
    for(int j=0;j<4;j++)
    {
      dt0.costs[i][j]=999;
    }
  }
   for(int i=0;i<4;i++)
  {
  dt0.costs[0][i]=0;
  }
  dt0.costs[1][1]=2;
  dt0.costs[2][2]=3;
  dt0.costs[3][3]=7;

  printf("At time t=%.3f, rinit0() called\n", clocktime);
  send0(NULL);
  printf("Distance table initialized: yes\n");
  printdt0(&dt0);


}


void rtupdate0(struct rtpkt *rcvdpkt)
{

  int sid=rcvdpkt->sourceid;
  int did=rcvdpkt->destid;
  int *mic=rcvdpkt->mincost;

  if(did!=0)
  {
    return;
  }

  int old_mincost[4];

  for(int i=0;i<4;i++)
  {
    int count=9999;
    for(int j=0;j<4;j++)
    {
      count=min0(count,dt0.costs[i][j]);
    }
    old_mincost[i]=count;
  }

    for(int i=0;i<4;i++)
    {
    dt0.costs[i][sid]=min0(dt0.costs[i][sid],dt0.costs[sid][sid]+mic[i]);
    }
 

  printf("At time t=%.3f, rtupdate0() called, sender=%d\n", clocktime, sid);
  printdt0(&dt0);
  send0(old_mincost);


  return;
}

void printdt0(struct distance_table *dtptr)
{
  printf("                via     \n");
  printf("   D0 |    1     2    3 \n");
  printf("  ----|-----------------\n");
  printf("     1|  %3d   %3d   %3d\n",dtptr->costs[1][1],
	 dtptr->costs[1][2],dtptr->costs[1][3]);
  printf("dest 2|  %3d   %3d   %3d\n",dtptr->costs[2][1],
	 dtptr->costs[2][2],dtptr->costs[2][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][1],
	 dtptr->costs[3][2],dtptr->costs[3][3]);
}

void linkhandler0(int linkid,int newcost)
{
  for(int i=0;i<4;i++)
  {
    if(i==0)
    {
      continue;
    }
    dt0.costs[i][linkid]=dt0.costs[i][linkid]-dt0.costs[linkid][linkid]+newcost;
    //由于假设邻居都很对，不成环，因此这里只需要更新改变的边改变的权值即可。这样就单纯通告了权值改变这个事，但没多余改变。
  }

  printf("At time t=%.3f, linkhandler0() called\n", clocktime);
  printf("Distance table changed: yes\n");
  send0(NULL);
  printdt0(&dt0);

} 
  

/* called when cost from 0 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
	

/*
对于这个算法，在不考虑环路优化的情况下，也就是说，保证什么条件一定能够收敛。那就是只要保证在某一时刻，
每个节点与自己相邻节点的距离是正确的，那么之后只要有足够长的稳定时间，一定能够收敛。
当然遵循的规则是Dj(i)=min(c[j][s]+Ds(i))并且一旦有变化就通告，哪怕通告是错的。
就是说，错误一定会导致环来解决，会不断叠加最后脱离。但如果是故意错误通告，那就无法叠加。也就是说阻断了环的增长。
所以说算得上故意错误的只有权值改变，和改变后对最小值的取舍（因为此时不知道这条路是不是真实的）。而只要没有了外部干涉。
最终算法一定会收敛。
*/

