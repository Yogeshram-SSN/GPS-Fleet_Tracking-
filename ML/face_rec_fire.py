import numpy as np
import pandas as pd
import cv2
import firebase_admin
import google.cloud
from firebase_admin import credentials, firestore

from insightface.app import FaceAnalysis
from sklearn.metrics import pairwise

import ast

collection_name='academy:register'
'''store=firestore.client()
cred = credentials.Certificate("./ServiceAccountKey.json")
app = firebase_admin.initialize_app(cred)'''

faceapp=FaceAnalysis(name='buffalo_sc',root='insightface_model',providers=['CPUExecutionProvider'])
faceapp.prepare(ctx_id=0,det_size=(640,640),det_thresh=0.5)

#ML Search Algorithm

def ml_search_algorithm(dataframe, feature_column, test_vector, name_role=['Name', 'Role'], thresh=0.5):
    # Step 1: Copy the dataframe to avoid modifying the original
    dataframe1 = dataframe.copy()

    # Step 2: Convert stored string embeddings to NumPy arrays
    dataframe1[feature_column] = dataframe1[feature_column].apply(
        lambda x: np.array(ast.literal_eval(x), dtype=np.float32) if isinstance(x, str) else x
    )

    # Convert list of embeddings to a NumPy array
    X_list = dataframe1[feature_column].tolist()
    x = np.asarray(X_list)

    # Ensure test_vector is a NumPy array
    if isinstance(test_vector, str):
        test_vector = np.array(ast.literal_eval(test_vector), dtype=np.float32)

    # Reshape test_vector
    test_vector = test_vector.reshape(1, -1)

    # Step 3: Calculate cosine similarity
    similar = pairwise.cosine_similarity(x, test_vector)
    similar_arr = np.array(similar).flatten()

    # Add similarity scores to dataframe
    dataframe1['cosine'] = similar_arr

    # Step 4: Filter data based on similarity threshold
    data_filter = dataframe1.query(f'cosine >= {thresh}')

    if len(data_filter) > 0:
        data_filter.reset_index(drop=True, inplace=True)
        argmax = data_filter['cosine'].idxmax()  # Use idxmax() instead of argmax()
        name, role = data_filter.loc[argmax, name_role]
    else:
        name, role = 'Unknown', 'Unknown'

    return name, role
#step 5  Get the person name
def face_prediction(test_image,dataframe,feature_column,name_role=['Name','Role'],thresh=0.5):
    # step 1 tke the test image and apply to insight faceA
    results=faceapp.get(test_image)
    test_copy=test_image.copy()
    
    # steep 2 use for loop and extract the embedding and pass to ml algorithm
    for res in results:
        x1,y1,x2,y2=res['bbox'].astype(int)
        embeddings=res['embedding']
    
        person_name,person_role=ml_search_algorithm(dataframe,feature_column,test_vector=embeddings,
                                                   name_role=name_role,thresh=thresh)
        if(person_name=='Unknown'):
            color=(0,0,255)
        else:
            color=(0,255,0)
        cv2.rectangle(test_copy,(x1,y1),(x2,y2),color)
    
        text_gen=person_name
        cv2.putText(test_copy,text_gen,(x1,y1),cv2.FONT_HERSHEY_DUPLEX,0.7,(0,255,0),1)
    return test_copy


