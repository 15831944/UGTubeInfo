// TubeInfo.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "TubeInfo.h"
#include  <vector>
#include  <uf.h>
#include  <uf_ui.h>
#include  <uf_obj.h>
#include  <uf_exit.h>
#include  <uf_modl.h>
#include  <uf_part.h>
#include  <uf_assem.h>
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CTubeInfoApp

BEGIN_MESSAGE_MAP(CTubeInfoApp, CWinApp)
END_MESSAGE_MAP()


// CTubeInfoApp ����

CTubeInfoApp::CTubeInfoApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CTubeInfoApp ����

CTubeInfoApp theApp;
bool IsArcInLine(tag_t arc_tag, tag_t line_tag)//�ж�Բ���Ƿ��ж˵���ֱ����
{
	double min_dist ;
    double pt_on_obj1 [ 3 ] ;
    double pt_on_obj2 [ 3 ] ;
	UF_MODL_ask_minimum_dist(arc_tag, line_tag, 0, NULL, 0, NULL, &min_dist, pt_on_obj1, pt_on_obj2);
	if(min_dist<0.00001)
		return true;
	else
		return false;
}

double Cal_distance(double a[3],double b[3])//�����֮��ľ���
{
	double distance = pow ((pow((a[0]-b[0]),2) + pow((a[1]-b[1]),2) + pow((a[2]-b[2]),2)) , 0.5);
	return distance;
}
struct lineAll
{
	tag_t line_tag;
	double start_point[3];
	double end_point[3];
};
struct arcAll
{
	tag_t arc_tag;
	double start_point[3];
	double end_point[3];
	double radius;
};
extern "C" DllExport void ufsta(char *param, int *returnCode, int rlen)
{
	if(UF_initialize()!=0)
	{
		return;
	}
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	vector< lineAll > lines;
	vector< arcAll > arcs;
	vector< lineAll > lines_sort;
	vector< arcAll > arcs_sort;
	lines.clear();
	arcs.clear();

	/*����ֱ��*/
	tag_t display_tag = UF_PART_ask_display_part();
	tag_t line_tag = NULL_TAG;
	UF_OBJ_cycle_objs_in_part(display_tag,UF_line_type,&line_tag);
	while ( line_tag != NULL_TAG )
	{
		lineAll line_1;
		line_1.line_tag = line_tag;
		UF_CURVE_line_s line_coords;
		UF_CURVE_ask_line_data(line_tag,&line_coords);
		for ( int j=0; j<3; j++ )
		{
		    line_1.start_point[j] = line_coords.start_point[j];
			line_1.end_point[j] = line_coords.end_point[j];
		}
		lines.push_back(line_1);
		UF_OBJ_cycle_objs_in_part(display_tag,UF_line_type,&line_tag);
	}

	/*����Բ��*/
	tag_t arc_tag = NULL_TAG;
	UF_OBJ_cycle_objs_in_part(display_tag,UF_circle_type,&arc_tag);
    while ( arc_tag != NULL_TAG )
	{
		arcAll arc_1;
		arc_1.arc_tag = arc_tag;
		UF_CURVE_arc_s arc_info;
	    UF_CURVE_ask_arc_data(arc_tag,&arc_info);
		arc_1.radius = arc_info.radius;
		double point_start [3] ;
		double point_end [3];
        double tangent [3] ;
        double p_norm [3] ;
        double b_norm [3] ;
        double torsion ;
        double rad_of_cur ;
		UF_MODL_ask_curve_props(arc_tag,0,point_start,tangent,p_norm,b_norm,&torsion,&rad_of_cur);
		UF_MODL_ask_curve_props(arc_tag,1,point_end,tangent,p_norm,b_norm,&torsion,&rad_of_cur);
		for ( int j=0; j<3; j++ )
		{
		    arc_1.start_point[j] = point_start[j];
			arc_1.end_point[j] = point_end[j];
		}
		arcs.push_back(arc_1);
		UF_OBJ_cycle_objs_in_part(display_tag,UF_circle_type,&arc_tag);
	}
	/*�ҳ���һ��ֱ��,�����㷨Ϊ���ÿһ��ֱ�߱���Բ����������Բ������ֻ��һ����Ϊ��һ��ֱ��*/
	lineAll line_first;
	for ( int i=0; i<lines.size(); i++)
	{
		double nInter = 0;
		for ( int j=0; j<arcs.size(); j++ )
		{
			if (IsArcInLine(arcs[j].arc_tag, lines[i].line_tag))
			//if (((abs( lines[i].start_point[0] - arcs[j].start_point[0] )<0.0001) && (abs( lines[i].start_point[1] - arcs[j].start_point[1] )<0.0001) && (abs( lines[i].start_point[2] - arcs[j].start_point[2] )<0.0001)) || 
				//((abs( lines[i].start_point[0] - arcs[j].end_point[0] )<0.0001) && (abs( lines[i].start_point[1] - arcs[j].end_point[1] )<0.0001) && (abs( lines[i].start_point[2] - arcs[j].end_point[2] )<0.0001)))
			{
				nInter++;
			}
			//if (((abs( lines[i].end_point[0] - arcs[j].start_point[0] )<0.0001) && (abs( lines[i].end_point[1] - arcs[j].start_point[1] )<0.0001) && (abs( lines[i].end_point[2] - arcs[j].start_point[2] )<0.0001)) || 
				//((abs( lines[i].end_point[0] - arcs[j].end_point[0] )<0.0001) && (abs( lines[i].end_point[1] - arcs[j].end_point[1] )<0.0001) && (abs( lines[i].end_point[2] - arcs[j].end_point[2] )<0.0001)))
			//{
				//nInter++;
			//}
		}
		if ( nInter == 1 )
		{
			line_first = lines[i];
			break;
		}
	}
	/*�Ժ���Բ����ֱ�߽�������*/
	lines_sort.push_back(line_first);
	lineAll line_last;
	line_last = line_first;
	arcAll arc_last;
	arc_last.arc_tag = NULL_TAG;
	for ( int k=0; k<arcs.size(); k++ )
	{
	    for ( int i=0; i<arcs.size(); i++ )
	    {
		    //if ((((abs(line_last.start_point[0] - arcs[i].start_point[0])<0.0001) && (abs(line_last.start_point[1] - arcs[i].start_point[1])<0.0001) && (abs(line_last.start_point[2] - arcs[i].start_point[2])<0.0001)) ||
			    //((abs(line_last.end_point[0] - arcs[i].start_point[0])<0.0001) && (abs(line_last.end_point[1] - arcs[i].start_point[1])<0.0001) && (abs(line_last.end_point[2] - arcs[i].start_point[2])<0.0001))||
			    //((abs(line_last.start_point[0] - arcs[i].end_point[0])<0.0001) && (abs(line_last.start_point[1] - arcs[i].end_point[1])<0.0001) && (abs(line_last.start_point[2] - arcs[i].end_point[2])<0.0001))||
			    //((abs(line_last.end_point[0] - arcs[i].end_point[0])<0.0001) && (abs(line_last.end_point[1] - arcs[i].end_point[1])<0.0001) && (abs(line_last.end_point[2] - arcs[i].end_point[2])<0.0001))) && ( arc_last.arc_tag != arcs[i].arc_tag ))
			if (IsArcInLine(arcs[i].arc_tag,line_last.line_tag) && ( arc_last.arc_tag != arcs[i].arc_tag ))
			{
			    arc_last = arcs[i];
			    arcs_sort.push_back(arc_last);
			    for ( int j=0; j<lines.size(); j++ )
			    {
				    if (line_last.line_tag != lines[j].line_tag )
				    {
					    //if (((abs(arc_last.start_point[0] - lines[j].start_point[0])<0.0001) && (abs(arc_last.start_point[1] - lines[j].start_point[1])<0.0001) && (abs(arc_last.start_point[2] - lines[j].start_point[2])<0.0001)) ||
			               //((abs(arc_last.end_point[0] - lines[j].start_point[0])<0.0001) && (abs(arc_last.end_point[1] - lines[j].start_point[1])<0.0001) && (abs(arc_last.end_point[2] - lines[j].start_point[2])<0.0001))||
			               //((abs(arc_last.start_point[0] - lines[j].end_point[0])<0.0001) && (abs(arc_last.start_point[1] - lines[j].end_point[1])<0.0001) && (abs(arc_last.start_point[2] - lines[j].end_point[2])<0.0001))||
			               //((abs(arc_last.end_point[0] - lines[j].end_point[0])<0.0001) && (abs(arc_last.end_point[1] - lines[j].end_point[1])<0.0001) && (abs(arc_last.end_point[2] - lines[j].end_point[2])<0.0001)))
						if(IsArcInLine(arc_last.arc_tag, lines[j].line_tag))
						{
						    line_last = lines[j];
						    lines_sort.push_back(line_last);
						    break;
					    }
				    }
			    }
			    break;
		    }
	    }
	}

	/*���ؼ���Ϣ��д���ļ�*/
	UF_UI_open_listing_window();
	char title[] = "�����Ϣ\n";
	UF_UI_write_listing_window(title);
	

	//��ܾ�
	tag_t feature_tag = NULL_TAG;
	UF_OBJ_cycle_objs_in_part(display_tag,UF_solid_type,&feature_tag);
	int t,subtype;
	UF_OBJ_ask_type_and_subtype(feature_tag,&t,&subtype);
	if(subtype == UF_solid_body_subtype)//�ж��Ƿ�����
	{
		uf_list_p_t list;
		UF_MODL_ask_body_edges(feature_tag,&list);//��ȡ��Ե����,�ܵ�����Բ
		int nlist;//��Ե���߸���
		UF_MODL_ask_list_count(list,&nlist);
	    double r[100];
		for( int k=0; k<nlist; k++)
		{
		    tag_t edge_tag;
			UF_MODL_ask_list_item(list,k,&edge_tag);
			UF_CURVE_arc_s arc_info;
			UF_CURVE_ask_arc_data(edge_tag,&arc_info);
		    r[k] = arc_info.radius;
		}
		char printinfo[255];
		sprintf(printinfo,"�⾶��%f, �ھ���%f\n", 2*max(r[0],r[1]), 2*min(r[0],r[1]));
		UF_UI_write_listing_window(printinfo);	
	}

	char column[] = "X          Y          Z          R\n";
	UF_UI_write_listing_window(column);
	//�����������������뾶��Ϣ
	for ( int i=0; i<arcs.size(); i++ )
	{
		UF_CURVE_arc_s arc_info;
		UF_CURVE_ask_arc_data(arcs_sort[i].arc_tag,&arc_info);
		
		UF_CURVE_intersect_info_s interpt_Info;
		double ref_point[3] = {0,0,0};
		UF_CURVE_intersect(lines_sort[i].line_tag,lines_sort[i+1].line_tag,ref_point,&interpt_Info);
		switch(interpt_Info.type_of_intersection)
        {
           case 1://3d����
			   {
			   char info[255];
			   if ( i == 0 )//��ʼ��
		       {
		           UF_CURVE_line_s line_coords;
		           UF_CURVE_ask_line_data(lines_sort[i].line_tag,&line_coords);
			       if (Cal_distance(line_coords.start_point,interpt_Info.curve_point)<Cal_distance(line_coords.end_point,interpt_Info.curve_point))
			       {
					   sprintf(info,"%6f, %6f, %6f, %6f\n",line_coords.end_point[0],line_coords.end_point[1],line_coords.end_point[2],0);
				   }
				   else
				   {
					   sprintf(info,"%6f, %6f, %6f, %6f\n",line_coords.start_point[0],line_coords.start_point[1],line_coords.start_point[2],0);
				   }
				   UF_UI_write_listing_window(info);
			   }
			   sprintf(info,"%6f, %6f, %6f, %6f\n",interpt_Info.curve_point[0],interpt_Info.curve_point[1],interpt_Info.curve_point[2],arc_info.radius);  
			   UF_UI_write_listing_window(info);
			   if ( i == arcs.size()-1 )
			   {
				   UF_CURVE_line_s line_coords;
		           UF_CURVE_ask_line_data(lines_sort[i+1].line_tag,&line_coords);
				   if (Cal_distance(line_coords.start_point,interpt_Info.curve_point)<Cal_distance(line_coords.end_point,interpt_Info.curve_point))
			       {
					   sprintf(info,"%6f, %6f, %6f, %6f\n",line_coords.end_point[0],line_coords.end_point[1],line_coords.end_point[2],0);
				   }
				   else
				   {
					   sprintf(info,"%6f, %6f, %6f, %6f\n",line_coords.start_point[0],line_coords.start_point[1],line_coords.start_point[2],0);
				   }
				   UF_UI_write_listing_window(info);
			   }
			   }
			   break;
		   default:
			   break;
		}
	}
	UF_terminate();
}

// CTubeInfoApp ��ʼ��

BOOL CTubeInfoApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
