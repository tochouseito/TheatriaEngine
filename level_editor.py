import bpy
import math
import bpy_extras
import gpu
import gpu_extras
import copy
import mathutils
import json
import uuid
from gpu_extras.batch import batch_for_shader
from gpu.types import GPUBatch, GPUVertBuf, GPUIndexBuf
from bpy.types import Operator, Panel, PropertyGroup
#ブレンダ―に登録するアドオン情報
bl_info = {
    "name": "レベルエディタ",
    "author": "Chosei Yara",
    "version": (1, 0),
    "blender": (4, 2, 0),
    "location": "",
    "description": "レベルエディタ",
    "warning": "",
    "category": "Object",
    "tracker_url": "",
}


#アドオン有効時コールバック
def register():
    # Blenderにクラスを登録
    for cls in classes:
        bpy.utils.register_class(cls)
        
    #メニューに項目を追加
    bpy.types.TOPBAR_MT_editor_menus.append(TOPBAR_MT_my_menu.submenu)
    #3Dビューに描画関数を追加
    DrawCollider.handle =bpy.types.SpaceView3D.draw_handler_add(DrawCollider.draw_collider,(),"WINDOW","POST_VIEW")
    print("レベルエディタが有効化されました")

#アドオン無効時コールバック
def unregister():
    #メニューから項目を削除
    bpy.types.TOPBAR_MT_editor_menus.remove(TOPBAR_MT_my_menu.submenu)
    #3Dビューから描画関数を削除
    bpy.types.SpaceView3D.draw_handler_remove(DrawCollider.handle,"WINDOW")
    
    # Blenderからクラスを削除
    for cls in classes:
        bpy.utils.unregister_class(cls)
    print("レベルエディタが無効化されました")
    
# menuDraw
def draw_menu_manual(self,context):
    #self : 呼び出し元のクラスインスタンス。C++でいうthisポインタ
    #context : カーソルを合わせた時のポップアップのカスタマイズなどに使用

    #トップバーの「エディタメニュー」に項目（オペレーター）を追加
    self.layout.operator("wm.url_open_preset",text="Manual",icon='HELP')

def convert_vec_blender_to_dx(v: mathutils.Vector) -> list[float]:
    """位置・スケール用：X,Y,Z → -Z,X,Y に変換"""
    return [v.y, v.z, -v.x]

def convert_quaternion_blender_to_dx(q: mathutils.Quaternion) -> mathutils.Quaternion:
    """回転用：クォータニオンをDirectX用に変換"""
    mat = q.to_matrix().to_4x4()
    converted = mathutils.Matrix((
        (-mat[2][0], mat[0][0], mat[1][0], 0),
        (-mat[2][1], mat[0][1], mat[1][1], 0),
        (-mat[2][2], mat[0][2], mat[1][2], 0),
        (0, 0, 0, 1)
    ))
    return converted.to_quaternion()
    
#トップバーの拡張メニュー
class TOPBAR_MT_my_menu(bpy.types.Menu):
    #Blenderがクラスを識別する為の固有の文字列
    bl_idname="TOPBAR_MT_my_menu"
    #メニューのラベルとして表示される文字列
    bl_label="MyMenu"
    #著者表示用の文字列
    bl_description="拡張メニュー by "+bl_info["author"]
    
    #サブメニューの描画
    def draw(self,context):
        
        #トップバーの「エディターメニュー」に項目（オペレーター）を追加
        self.layout.operator(MYADDON_OT_stretch_vertex.bl_idname,
        text=MYADDON_OT_stretch_vertex.bl_label)

        #トップバーの「エディターメニュー」に項目（オペレーター）を追加
        self.layout.operator(MYADDON_OT_create_ico_sphere.bl_idname,
        text=MYADDON_OT_create_ico_sphere.bl_label)

        #トップバーの「エディターメニュー」に項目（オペレーター）を追加
        self.layout.operator(MYADDON_OT_export_scene.bl_idname,
        text=MYADDON_OT_export_scene.bl_label)

    #既存のメニューにサブメニューを追加
    def submenu(self,context):
        
        #ID指定でサブメニューを追加
        self.layout.menu(TOPBAR_MT_my_menu.bl_idname)

#オペレータ 頂点を伸ばす
class MYADDON_OT_stretch_vertex(bpy.types.Operator):
    bl_idname="myaddon.myaddon_ot_stretch_vertex"
    bl_label="頂点を伸ばす"
    bl_description="頂点座標を引っ張って伸ばします"
    #リデゥ、アンドゥ可能オプション
    bl_options={'REGISTER','UNDO'}
    
    #メニューを実行したときに呼ばれるコールバック関数
    def execute(self,context):
        bpy.data.objects["Cube"].data.vertices[0].co.x+=1.0
        print("頂点を伸ばしました")
        #オペレータの命令終了を通知
        return {'FINISHED'}
    
#オペレータ IOC球生成
class MYADDON_OT_create_ico_sphere(bpy.types.Operator):
    bl_idname="myaddon.myaddon_ot_create_object"
    bl_label="ICO球生成"
    bl_description="ICO球を生成します"
    bl_options={'REGISTER','UNDO'}

    # メニューを実行したときに呼ばれる関数
    def execute(self,context):
        bpy.ops.mesh.primitive_ico_sphere_add()
        print("ICO球を生成しました。")

        return {'FINISHED'}

#オペレータ シーン出力
class MYADDON_OT_export_scene(bpy.types.Operator,bpy_extras.io_utils.ExportHelper):
    bl_idname="myaddon.myaddon_ot_export_scene"
    bl_label="シーン出力"
    bl_description="シーン情報をExportします"
    #出力するファイルの拡張子
    filename_ext=".json"

    def write_and_print(self, file, text):
        print(text)
        file.write(text)
        file.write('\n')
    
    def parse_scene_recursive(self,file,object,level):
        """シーン解析用再起関数"""

        #深さ分インデントする（タブを挿入）
        indent=''
        for i in range(level):
            indent +="\t"

        #オブジェクト名書き込み
        self.write_and_print(file, indent+object.type)
        #ローカルトランスフォーム行列から平行移動、回転、スケーリングを抽出
        #型は　Vector、Quaternion、Vector
        trans,rot,scale=object.matrix_local.decompose()
        #回転をQuaternionから　Euler （3軸での回転角）に変換
        rot=rot.to_euler()
        #ラジアンから度数法に変換
        rot.x=math.degrees(rot.x)
        rot.y=math.degrees(rot.y)
        rot.z=math.degrees(rot.z)
        #トランスフォーム情報を表示
        self.write_and_print(file,indent+"T %f %f %f" % (trans.x, trans.y, trans.z))
        self.write_and_print(file,indent+"R %f %f %f" %(rot.x,rot.y,rot.z))
        self.write_and_print(file,indent+"S %f %f %f" %(scale.x,scale.y,scale.z))
        #カスタムプロパティ'file_name'
        if "file_name" in object:
            self.write_and_print(file,indent+"N %s" % object["file_name"])
        #カスタムプロパティ'collision
        if "collider" in object:
            self.write_and_print(file,indent+"C %s" % object["collider"])
            temp_str=indent+"CC %f %f %f"
            temp_str%=(object["collider_center"][0],object["collider_center"][1],object["collider_center"][2])
            self.write_and_print(file,temp_str)
            temp_str=indent+"CS %f %f %f"
            temp_str%=(object["collider_size"][0],object["collider_size"][1],object["collider_size"][2])
            self.write_and_print(file,temp_str)
            
        self.write_and_print(file,indent+'END')
        self.write_and_print(file,'')

        file.write("\n")

        #子ノードへ進む（深さが１下がる）
        for child in object.children:
            self.parse_scene_recursive(file,child,level+1)

    def parse_scene_recursive_json(self,
    data_parent,object,level):
        
        #シーンのオブジェクト一個分のjsonオブジェクト生成
        json_object=dict()
        #オブジェクト種類
        json_object["type"]=object.type
        #オブジェクト名
        json_object["name"]=object.name

        #その情報をパック
        #オブジェクトのローカルトランスフォームから
        #平行移動、回転、スケールを抽出
        trans,rot,scale=object.matrix_local.decompose()
        #回転をQuaternionからEuler（3軸での回転角）に変換
        rot=rot.to_euler()
        #ラジアンから度数法に変換
        rot.x=math.degrees(rot.x)
        rot.y=math.degrees(rot.y)
        rot.z=math.degrees(rot.z)
        #トランスフォーム情報をディクショナリに登録
        transform=dict()
        transform["translation"]=(trans.x,trans.y,trans.z)
        transform["rotation"]=(rot.x,rot.y,rot.z)
        transform["scaling"]=(scale.x,scale.y,scale.z)
        #まとめて一個分のjsonオブジェクトに登録
        json_object["transform"]=transform
        #カスタムプロパティ'file_name'
        if "file_name" in object:
            json_object["file_name"]=object["file_name"]

        #カスタムプロパティ'collider'
        if "collider" in object:
            collider=dict()
            collider["type"]=object["collider"]
            collider["center"]=object["collider_center"].to_list()
            collider["size"]=object["collider_size"].to_list()
            json_object["collider"]=collider

        #1個分のjsonオブジェクトを親オブジェクトに登録
        data_parent.append(json_object)

        #直接の子供リストを走査
        #子ノードがあれば
        if len(object.children)>0:
            #子ノードリストを作成
            json_object["children"]=list()

            #子ノードへ進む（深さが1下がる）
            for child in object.children:
                self.parse_scene_recursive_json(json_object["children"],child,level+1)


    def export_json(self):
        """JSON形式でファイルに出力"""

        #保存する情報をまとめる
        json_object_root=dict()

        #ノード名
        json_object_root["name"]="scene"
        #オブジェクトリストを作成
        json_object_root["objects"]=list()

        #シーン内のオブジェクト走査してパック
        #シーン内の全オブジェクトについて
        for object in bpy.context.scene.objects:

            #親オブジェクトがあるものはスキップ（代わりに親から呼び出すから）
            if(object.parent):
                continue

            #シーン直下のオブジェクトをルートノード（深さ0）とし、再起関数で走査
            self.parse_scene_recursive_json(json_object_root["objects"],object,0)

        #オブジェクトをJSON文字列にエンコード（改行・インデント付き）
        json_text=json.dumps(json_object_root,ensure_ascii=False,cls=json.JSONEncoder,indent=4)

        #コンソールに表示してみる
        print(json_text)

        #ファイルをテキスト形式で書き出し用にオープン
        #スコープを抜けると自動的にクローズされる
        with open(self.filepath,"wt",encoding="utf-8")as file:

            #ファイルに文字列を書き込む
            file.write(json_text)       

    def execute(self,context):

        #print("シーン情報をExportします")

        #ファイルに出力
        #self.export_json()

        #print("シーン情報をExportしました")
        #self.report({'INFO'},"シーン情報をExportしました")

        #return {'FINISHED'}
        print("ChoEngine形式でシーンをExportします")
        self.export_choengine_format()
        self.report({'INFO'}, "ChoEngine形式でExportしました")
        return {'FINISHED'}
    def export(self):
        """ファイルに出力"""

        print("シーン情報出力開始… %r" % self.filepath)

        #ファイルをテキスト形式で書き出し用にオープン
        #スコープを抜けると自動的にクローズされる
        with open(self.filepath,"wt") as file:

            #ファイルに文字列を書き込む
            file.write("SCENE\n")

            #シーン内の全オブジェクトについて
            for object in bpy.context.scene.objects:
                
                #親オブジェクトがあるものはスキップ（代わりに親から呼び出すから
                if(object.parent):
                    continue

                #シーン直下のオブジェクトをルートノード（深さ0）とし、再起関数で走査
                self.parse_scene_recursive(file,object,0)

    def export_choengine_format(self):
        """ChoEngine形式のJSONファイルを書き出す"""
        import re

        # シーン名
        scene_name = bpy.path.display_name_from_filepath(bpy.data.filepath)
        if not scene_name:
            scene_name = "MainScene"

        json_root = {
            "fileType": "SceneFile",
            "sceneName": scene_name,
            "objects": []
        }

        for obj in bpy.context.scene.objects:
            if obj.parent:
                continue

            obj_json = {
                "name": obj.name,
                "type": "MeshObject" if obj.type == 'MESH' and obj.data else "Empty",
                "components": {}
            }

            # Transform
            trans, rot, scale = obj.matrix_local.decompose()
            euler = rot.to_euler()

            converted_trans = convert_vec_blender_to_dx(trans)
            converted_scale = convert_vec_blender_to_dx(scale)
            converted_rot = convert_quaternion_blender_to_dx(rot)

            obj_json["components"]["Transform"] = {
                "translation": converted_trans,
                "rotation": [converted_rot.x, converted_rot.y, converted_rot.z, converted_rot.w],
                "scale": converted_scale,
                "degrees": [math.degrees(euler.x), math.degrees(euler.y), math.degrees(euler.z)]
            }

            # MeshFilter & MeshRenderer
            if obj.type == 'MESH' and obj.data:
                base_name = re.sub(r"\.\d+$", "", obj.data.name)
                obj_json["components"]["MeshFilter"] = {
                    "modelName": base_name
                }
                obj_json["components"]["MeshRenderer"] = {
                    "visible": True
                }

            json_root["objects"].append(obj_json)

        json_text = json.dumps(json_root, ensure_ascii=False, indent=4)
        with open(self.filepath, "wt", encoding="utf-8") as file:
            file.write(json_text)


    
                
#パネル ファイル名
class OBJECT_PT_file_name(bpy.types.Panel):
    """オブジェクトのファイルネームパネル"""
    bl_idname="OBJECT_PT_file_name"
    bl_label="FileName"
    bl_space_type="PROPERTIES"
    bl_region_type="WINDOW"
    bl_context="object"

    #サブメニューの描画
    def draw(self,context):

        #パネルに項目を追加
        if "file_name" in context.object:
            #既にプロパティがあれば、プロパティを表示
            self.layout.prop(context.object,'["file_name"]',text=self.bl_label)
        else:
            #プロパティがなければ、プロパティ追加ボタンを表示
            self.layout.operator(MYADDON_OT_add_filename.bl_idname)

#オペレータ カスタムプロパティ['file_name']追加
class MYADDON_OT_add_filename(bpy.types.Operator):
    """オブジェクトのファイルネームパネル"""
    bl_idname="myaddon.myaddon_ot_add_filename"
    bl_label="FileName 追加"
    bl_description="['file_name']カスタムプロパティを追加します"
    bl_options={"REGISTER","UNDO"}

    def execute(self,context):

        #['file_name']カスタムプロパティを追加
        context.object["file_name"]=""

        return {"FINISHED"}
    
#コライダー描画
class DrawCollider:

    # 描画ハンドル
    handle=None
    #3Dビューに登録する描画関数
    def draw_collider():
        #頂点データ
        vertices={"pos":[]}
        #インデックスデータ
        indices=[]

        #各頂点の、オブジェクト中心からのオフセット
        offsets=[
                 [-0.5,-0.5,-0.5], #左下前
                 [+0.5,-0.5,-0.5], #右下前
                 [-0.5,+0.5,-0.5], #右下前
                 [+0.5,+0.5,-0.5], #右下前
                 [-0.5,-0.5,+0.5], #右下前
                 [+0.5,-0.5,+0.5], #右下前
                 [-0.5,+0.5,+0.5], #右下前
                 [+0.5,+0.5,+0.5], #右下前
        ]

        #立方体のX,Y,Z方向サイズ
        size=[2,2,2]

        #現在のシーンのオブジェクトリストを走査
        for object in bpy.context.scene.objects:

            #コライダープロパティがなければ、描画をスキップ
            if not "collider" in object:
                continue

            #中心点、サイズの変数を宣言
            center=mathutils.Vector((0,0,0))
            size=mathutils.Vector((2,2,2))

            #プロパティから値を取得
            center[0]=object["collider_center"][0]
            center[1]=object["collider_center"][1]
            center[2]=object["collider_center"][2]
            size[0]=object["collider_size"][0]
            size[1]=object["collider_size"][1]
            size[2]=object["collider_size"][2]

            #追加前の頂点数
            start =len(vertices["pos"])

            #Boxの8頂点分回す
            for offset in offsets:
                #オブジェクトの中心座標をコピー
                pos=copy.copy(center)
                #中心座標を基準に各頂点ごとにずらす
                pos[0]+=offset[0]*size[0]
                pos[1]+=offset[1]*size[1]
                pos[2]+=offset[2]*size[2]
                #ローカル座標からワールド座標に変換
                pos=object.matrix_world @ pos
                #頂点データリストに座標を追加
                vertices['pos'].append(pos)
                #前面を構成する辺の頂点インデックス
                indices.append([start+0,start+1])
                indices.append([start+2,start+3])
                indices.append([start+0,start+2])
                indices.append([start+1,start+3])
                #奥面を構成する辺の頂点インデックス
                indices.append([start+4,start+5])
                indices.append([start+6,start+7])
                indices.append([start+4,start+6])
                indices.append([start+5,start+7])
                #前と頂点をつなぐ辺の頂点インデックス
                indices.append([start+0,start+4])
                indices.append([start+1,start+5])
                indices.append([start+2,start+6])
                indices.append([start+3,start+7])

        #ビルトインのシェーダを取得
        shader=gpu.shader.from_builtin("UNIFORM_COLOR")

        #バッチを作成(引数:シェーダ、トポロジー、頂点データ、インデックスデータ)
        batch=gpu_extras.batch.batch_for_shader(shader,"LINES",vertices,indices=indices)

        # シェーダのパラメータ設定
        color=[0.5,1.0,1.0,1.0]
        shader.bind()
        shader.uniform_float("color",color)
        # 描画
        batch.draw(shader)

#オペレータ カスタムプロパティ['collider']を追加
class MYADDON_OT_add_collider(bpy.types.Operator):
    bl_idname="myaddon.myaddon_ot_add_collider"
    bl_label="コライダー 追加"
    bl_description="['collider']カスタムプロパティを追加します"
    bl_options={"REGISTER","UNDO"}
    def execute(self,context):

        #['collider']カスタムプロパティを追加
        context.object["collider"]="BOX"
        context.object["collider_center"]=mathutils.Vector((0,0,0))
        context.object["collider_size"]=mathutils.Vector((2,2,2))

        return {"FINISHED"}
    
# パネル コライダー
class OBJECT_PT_collider(bpy.types.Panel):
    bl_idname="OBJECT_PT_collider"
    bl_label="Collider"
    bl_space_type="PROPERTIES"
    bl_region_type="WINDOW"
    bl_context="object"

    #サブメニューの描画
    def draw(self,context):
        
        #パネルに項目を追加
        if "collider" in context.object:
            #既にプロパティがあれば、プロパティを表示
            self.layout.prop(context.object,'["collider"]',text="Type")
            self.layout.prop(context.object,'["collider_center"]',text="Center")
            self.layout.prop(context.object,'["collider_size"]',text="Size")
        else:
            #プログラムがなければ、プロパティ追加ボタンを表示
            self.layout.operator(MYADDON_OT_add_collider.bl_idname)
    
# Blenderに登録するリスト
classes=(
    MYADDON_OT_export_scene,
    MYADDON_OT_create_ico_sphere,
    MYADDON_OT_stretch_vertex,
    TOPBAR_MT_my_menu,
    MYADDON_OT_add_filename,
    OBJECT_PT_file_name,
    MYADDON_OT_add_collider,
    OBJECT_PT_collider,
)