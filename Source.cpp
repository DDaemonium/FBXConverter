#include <fbxsdk.h>
#pragma comment (lib, "libfbxsdk-md.lib")
#include <fstream>

int main() {
	std::ofstream out("C:\\Users\\user\\Desktop\\check.txt");
	// Change the following filename to a suitable filename value.
	const char* lFilename = "C:\\Users\\user\\Desktop\\002.fbx";

	// Initialize the SDK manager. This object handles all our memory management.
	FbxManager* lSdkManager = FbxManager::Create();

	// Create the IO settings object.
	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	// Create an importer using the SDK manager.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	// Use the first argument as the filename for the importer.
	if (!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings())) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		exit(-1);
	}

	// Create a new scene so that it can be populated by the imported file.
	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");

	// Import the contents of the file into the scene.
	lImporter->Import(lScene);

	// The file is imported; so get rid of the importer.
	lImporter->Destroy();
	// Print the nodes of the scene and their attributes recursively.
	// Note that we are not printing the root node because it should
	// not contain any attributes.
	FbxNode* lRootNode = lScene->GetRootNode();
	FbxNode *childNode(nullptr);
	FbxMesh *mesh(nullptr);
	int *indices;
	int numIndices;
	FbxGeometryElementNormal* normalEl;
	if (lRootNode) {
		for (int i = 0; i < lRootNode->GetChildCount(); i++) {
			childNode = lRootNode->GetChild(i);
			out << "-------------------------------------------------" << childNode->GetName() << '\n';
			mesh = childNode->GetMesh();
			if (mesh) {
				int numVerts = mesh->GetControlPointsCount();
				out << "Positions\n";
				for (int j = 0; j < numVerts; j++)
				{
					FbxVector4 vert = mesh->GetControlPointAt(j);
					out << (float)vert.mData[0] << ' ' << (float)vert.mData[1] << ' ' << (float)vert.mData[2] << '\n';
				}
				numIndices = mesh->GetPolygonVertexCount();
				indices = new int[numIndices];
				indices = mesh->GetPolygonVertices();
				out << "Indecis\n" << numIndices << '\n';
				for (int j(0); j < numIndices; j++) {
					out << indices[j] << ' ';
				}
				out << "\nNormals\n";
				normalEl = mesh->GetElementNormal();
				if (normalEl) {
					auto numNormals = mesh->GetPolygonCount() * 3;
					auto normals = new float[numNormals * 3];
					int vertexCounter(0);
					for (int polyCounter = 0; polyCounter < mesh->GetPolygonCount(); polyCounter++) {
						for (int i = 0; i < 3; i++) {
							FbxVector4 normal = normalEl->GetDirectArray().GetAt(vertexCounter);
							out << normal[0] << ' ' << normal[1] << ' ' << normal[2] << '\n';
							vertexCounter++;
						}
					}
				}
				out << "Texture Coords\n";
				bool unMaped;
				for (int Poly(0); Poly < mesh->GetPolygonCount(); Poly++) {
					auto NumVertices = mesh->GetPolygonSize(Poly);
					for (int Vertex(0); Vertex < NumVertices; Vertex++)
					{
						FbxVector2 fbxTexCoord;
						FbxStringList UVSetNameList;

						// Get the name of each set of UV coords
						mesh->GetUVSetNames(UVSetNameList);

						// Get the UV coords for this vertex in this poly which belong to the first UV set
						// Note: Using 0 as index into UV set list as this example supports only one UV set
						mesh->GetPolygonVertexUV(Poly, Vertex, UVSetNameList.GetStringAt(0), fbxTexCoord, unMaped);
						out << static_cast<float>(fbxTexCoord[0]) << ' ' << static_cast<float>(fbxTexCoord[1]) << '\n';
					}
				}
			}
		}
	}
	// Destroy the SDK manager and all the other objects it was handling.
	lSdkManager->Destroy();
	return 0;
}