
# Unreal Engine 5 Plugin: Asset Auditor

This tool is to allow mass auditing of Meshes. It should be able to output relevant information on all Meshes within the Unreal Project.


## Installation

You can install the plugin by dragging and dropping the `MeshAuditingTool` folder inside your `/Plugins/` folder then restart the editor to load the plugin.
## Usage

Once the editor has been restarted, there should be an Audit icon in the editor toolbar.
![Toolbar Screenshot](https://i.imgur.com/i2lOJrY.png)

You start the Audit by simply clicking the icon. This should open a window that will help you audit assets in the editor.
![Toolbar Screenshot](https://i.imgur.com/i2lOJrY.png)

There are various settings that you can use to tell the auditor how to audit assets.

- **Root Directory:**  Tells the auditor what folder to search from.
- **Include Static Mesh/Skeletal Mesh/Skeleton/Animation:** Check the type of asset you want to search for and audit.
- **Audit Type:** Indicates what type of audit you want.
  - `Individual`: Lists down all found assets individually and their data.
  - `Total`: Sums up all the data of all found assets.
  - `Average`: Gets the average count of all data in each particular asset type. 
- **Audit Meshes:** Starts the audit using the settings provided.



