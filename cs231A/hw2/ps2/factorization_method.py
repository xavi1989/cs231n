import numpy as np
from scipy.misc import imread
import matplotlib.pyplot as plt
import scipy.io as sio
import matplotlib.gridspec as gridspec
from epipolar_utils import *

'''
FACTORIZATION_METHOD The Tomasi and Kanade Factorization Method to determine
the 3D structure of the scene and the motion of the cameras.
Arguments:
    points_im1 - N points in the first image that match with points_im2
    points_im2 - N points in the second image that match with points_im1

    Both points_im1 and points_im2 are from the get_data_from_txt_file() method
Returns:
    structure - the structure matrix
    motion - the motion matrix
'''
def factorization_method(points_im1, points_im2):
    # TODO: Implement this method!
    # center the data
    center_im1 = np.mean(points_im1, axis = 0)
    center_im2 = np.mean(points_im2, axis = 0)

    points_im1_center = points_im1 - center_im1
    points_im2_center = points_im2 - center_im2

    # create measurement matrix
    D = np.zeros((4, points_im1.shape[0]))

    D[0:2, :] = points_im1_center.T[0:2, :]
    D[2:4, :] = points_im2_center.T[0:2, :]

    # svd
    U, s, VT = np.linalg.svd(D)

    motion = U[:, 0:3]
    eigen = np.zeros((3, 3))
    eigen[0, 0] = s[0]
    eigen[1, 1] = s[1]
    eigen[2, 2] = s[2]

    print VT[0:3, :].shape
    structure = eigen.dot(VT[0:3, :])

    return structure, motion


if __name__ == '__main__':
    for im_set in ['data/set1', 'data/set1_subset']:
        # Read in the data
        im1 = imread(im_set+'/image1.jpg')
        im2 = imread(im_set+'/image2.jpg')
        points_im1 = get_data_from_txt_file(im_set + '/pt_2D_1.txt')
        points_im2 = get_data_from_txt_file(im_set + '/pt_2D_2.txt')
        points_3d = get_data_from_txt_file(im_set + '/pt_3D.txt')
        assert (points_im1.shape == points_im2.shape)

        # Run the Factorization Method
        structure, motion = factorization_method(points_im1, points_im2)

        # Plot the structure
        fig = plt.figure()
        ax = fig.add_subplot(121, projection = '3d')
        scatter_3D_axis_equal(structure[0,:], structure[1,:], structure[2,:], ax)
        ax.set_title('Factorization Method')
        ax = fig.add_subplot(122, projection = '3d')
        scatter_3D_axis_equal(points_3d[:,0], points_3d[:,1], points_3d[:,2], ax)
        ax.set_title('Ground Truth')

        plt.show()
