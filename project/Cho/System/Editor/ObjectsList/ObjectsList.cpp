#include "PrecompiledHeader.h"
#include "ObjectsList.h"

#include"imgui.h"

#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"D3D12/RTVManager/RTVManager.h"
#include"D3D12/DrawExecution/DrawExecution.h"

// SceneManager
#include"Scene/SceneManager/SceneManager.h"

#include"ECS/GameObject/GameObject.h"

#include"Editor/EditorManager/EditorManager.h"

void ObjectsList::Initialize(
    ResourceViewManager* rvManager,
    RTVManager* rtvManager,
    DrawExecution* drawExe, 
    EntityManager* entityManager,
    ComponentManager* componentManager,
    SystemManager* systemManager,
    PrefabManager* prefabManager,
    SceneManager* sceneManager,
    EditorManager* editManager
)
{
    // D3D12
    rvManager_ = rvManager;
    rtvManager_ = rtvManager;
    drawExe_ = drawExe;

    // ECS
    entityManager_ = entityManager;
    componentManager_ = componentManager;
    systemManager_ = systemManager;
    prefabManager_ = prefabManager;

    // SceneManager
    sceneManager_ = sceneManager;

    editManager_ = editManager;
}

void ObjectsList::Update()
{
    // 移動を無効にするフラグ
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;
    ImGui::Begin("GameObjectList",nullptr,windowFlags);

    selectObject(sceneManager_->GetGameObjects());      // ベースオブジェクトを表示

    ImGui::End();
}

void ObjectsList::selectObject(const std::unordered_map<std::string, std::unique_ptr<GameObject>>& objectList)
{
    static bool editing = false;
    static char inputBuffer[128] = ""; // 入力用バッファ
    static std::string editingName;   // 現在編集中のオブジェクト名
    std::vector<std::pair<std::string, std::string>> renameRequests;
    static ObjectType renameType = ObjectType::Object;

    for (auto& pair : objectList) {
        const std::string& name = pair.first;
        GameObject* gameObject = pair.second.get();

        if (editing && editingName == name) {
            // 編集モード: InputTextを表示
            if (ImGui::InputText("##edit", inputBuffer, IM_ARRAYSIZE(inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                std::string newName(inputBuffer);

                // 名前が空でない場合に更新
                if (!newName.empty()) {
                    renameRequests.push_back({ name, newName });  // 後で変更するために記録
                    renameType = pair.second->GetObjectType();

                    editing = false;  // 編集終了
                }
            }

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemActive()) {
                editing = false;
            }
        }
        else {
            //// 通常モード: Selectableを表示
            //if (ImGui::Selectable(name.c_str())) {
            //    editManager_->SetSelectedGOName(name);
            //    editManager_->SetSelectedGO(pair.second.get());  // 選択したオブジェクトを保持
            //}

            //// ダブルクリックで編集モードに切り替え
            //if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            //    editing = true;
            //    editingName = name;                       // 編集中の名前を設定
            //    strncpy_s(inputBuffer, IM_ARRAYSIZE(inputBuffer), name.c_str(), _TRUNCATE); // 現在の名前をバッファにコピー

            //}

            // 親ノードとしてツリーノードを表示
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
            if (editManager_->GetSelectedGO() == gameObject) {
                flags |= ImGuiTreeNodeFlags_Selected; // 選択中のオブジェクトをハイライト
            }

            bool isTreeOpen = ImGui::TreeNodeEx(name.c_str(), flags);

            // 親ノードがクリックされた場合の処理
            if (ImGui::IsItemClicked()) {
                editManager_->SetSelectedGOName(name);
                editManager_->SetSelectedGO(gameObject);
            }

            // ダブルクリックで編集モードに切り替え
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                editing = true;
                editingName = name;                       // 編集中の名前を設定
                strncpy_s(inputBuffer, IM_ARRAYSIZE(inputBuffer), name.c_str(), _TRUNCATE); // 現在の名前をバッファにコピー

            }

            // ツリーノードが展開されている場合
            if (isTreeOpen) {
                std::optional<std::reference_wrapper<EffectComponent>> effectComp = componentManager_->GetComponent<EffectComponent>(gameObject->GetEntityID());
                if (effectComp) {
                    EffectComponent& effect = effectComp.value();

                    // 子ノードを取得
                    auto& children = effect.effectNodes;
                    for (size_t i = 0; i < children.size(); ++i) {
                        DrawEffectNodeTree(children[i], i, editManager_);
                    }
                }
                ImGui::TreePop(); // 親ノードを閉じる
            }
        }
    }
    // ループ終了後に名前を変更
    for (const auto& request : renameRequests) {
        const std::string& oldName = request.first;
        const std::string& newName = request.second;

        std::string name;

        name = sceneManager_->GameObjectListRename(newName, oldName);  // 名前変更
        editManager_->SetSelectedGOName(name);
        editManager_->SetSelectedGO(sceneManager_->GetGameObject(name));  // 選択したオブジェクトを保持
    }
}

void ObjectsList::DrawEffectNodeTree(EffectNode& node, size_t parentIndex, EditorManager* editManager) {
    // ノードの名前にインデックスを付加して一意にする
    std::string uniqueLabel = node.common.name + "##" + std::to_string(parentIndex);

    // ノードを表示
    if (ImGui::TreeNodeEx(uniqueLabel.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth)) {
        // ノードがクリックされた場合の処理
        if (ImGui::IsItemClicked()) {
            editManager->SetSelectedEffectNode(&node);
        }

        // 子ノードを再帰的に表示
        for (size_t i = 0; i < node.children.size(); ++i) {
            DrawEffectNodeTree(node.children[i], i, editManager);
        }

        ImGui::TreePop(); // ノードを閉じる
    }
}

