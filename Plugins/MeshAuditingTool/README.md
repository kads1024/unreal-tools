
# Unreal Engine 5 Plugin: Asset Auditor

This tool is to allow mass auditing of Meshes. It should be able to output relevant information on all Meshes within the Unreal Project.


## Installation

You can install the plugin by dragging and dropping the `MeshAuditingTool` folder inside your `/Plugins/` folder then restart the editor to load the plugin.
## Usage

Once the editor has been restarted, there should be an Audit icon in the editor toolbar.
![Toolbar Screenshot](https://i.imgur.com/i2lOJrY.png)

You start the Audit by simply clicking the icon. This should open a window that will help you audit assets in the editor.
![Toolbar Screenshot](https://i.imgur.com/vuilF7A.png)

There are various settings that you can use to tell the auditor how to audit assets.

- **Root Directory:**  Tells the auditor what folder to search from. To search everywhere, put in `/Game/`
- **Include Static Mesh/Skeletal Mesh/Skeleton/Animation:** Check the type of asset you want to search for and audit.
- **Audit Type:** Indicates what type of audit you want.
  - `Individual`: Lists down all found assets individually and their data.
  - `Total`: Sums up all the data of all found assets.
  - `Average`: Gets the average count of all data in each particular asset type. 
- **Audit Meshes:** Starts the audit using the settings provided.

## Example Output

### Individual Results
```
LogAssetAuditor: Display: ------------------INDIVIDUAL AUDIT RESULTS------------------
LogAssetAuditor: Display: ASSET NAME: Floor_400x400 --------------------------
LogAssetAuditor: Display: ASSET TYPE: STATIC MESH
LogAssetAuditor: Display:      Number of LODs: 1
LogAssetAuditor: Display:         Number of Triangles on LOD[0]: 12
LogAssetAuditor: Display:      Number of Material Slots: 1
LogAssetAuditor: Display: ------------------------------------------------------------
LogAssetAuditor: Display: ASSET NAME: SM_Chair --------------------------
LogAssetAuditor: Display: ASSET TYPE: STATIC MESH
LogAssetAuditor: Display:      Number of LODs: 1
LogAssetAuditor: Display:         Number of Triangles on LOD[0]: 1782
LogAssetAuditor: Display:      Number of Material Slots: 1
LogAssetAuditor: Display: ------------------------------------------------------------
LogAssetAuditor: Display: ASSET NAME: SM_Statue --------------------------
LogAssetAuditor: Display: ASSET TYPE: STATIC MESH
LogAssetAuditor: Display:      Number of LODs: 1
LogAssetAuditor: Display:         Number of Triangles on LOD[0]: 4024
LogAssetAuditor: Display:      Number of Material Slots: 2
LogAssetAuditor: Display: ------------------------------------------------------------
.
.
.
.
.
LogAssetAuditor: Display: ------------------------------------------------------------
LogAssetAuditor: Display: ASSET NAME: Shape_Cone --------------------------
LogAssetAuditor: Display: ASSET TYPE: STATIC MESH
LogAssetAuditor: Display:      Number of LODs: 1
LogAssetAuditor: Display:         Number of Triangles on LOD[0]: 284
LogAssetAuditor: Display:      Number of Material Slots: 1
LogAssetAuditor: Display: ------------------------------------------------------------
LogAssetAuditor: Display: -------------------END OF AUDIT-----------------------------
```

### Total Results
```
LogAssetAuditor: Display: ------------------TOTAL AUDIT RESULTS------------------
LogAssetAuditor: Display: STATIC MESH:
LogAssetAuditor: Display:      Number of LODs: 51
LogAssetAuditor: Display:      Number of Material Slots: 55
LogAssetAuditor: Display: SKELETAL MESH:
LogAssetAuditor: Display:      Number of LODs: 1
LogAssetAuditor: Display:      Number of Material Slots: 2
LogAssetAuditor: Display: SKELETON:
LogAssetAuditor: Display:      Number of LODs: 0
LogAssetAuditor: Display:      Number of Bones: 68
LogAssetAuditor: Display: ANIMATION:
LogAssetAuditor: Display:      Number of Key Frames: 4241
LogAssetAuditor: Display: -------------------END OF AUDIT-----------------------------
```

### Average Results
```
LogAssetAuditor: Display: ------------------AVERAGE AUDIT RESULTS------------------
LogAssetAuditor: Display: STATIC MESH:
LogAssetAuditor: Display:      Number of LODs: 1
LogAssetAuditor: Display:      Number of Material Slots: 1
LogAssetAuditor: Display: SKELETAL MESH:
LogAssetAuditor: Display:      Number of LODs: 1
LogAssetAuditor: Display:      Number of Material Slots: 2
LogAssetAuditor: Display: SKELETON:
LogAssetAuditor: Display:      Number of LODs: 0
LogAssetAuditor: Display:      Number of Bones: 68
LogAssetAuditor: Display: ANIMATION:
LogAssetAuditor: Display:      Number of Key Frames: 68
LogAssetAuditor: Display: -------------------END OF AUDIT-----------------------------
```

