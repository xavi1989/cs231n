import numpy as np
from scipy.misc import imread
import matplotlib.pyplot as plt
import scipy.io as sio
from epipolar_utils import *

'''
LLS_EIGHT_POINT_ALG  computes the fundamental matrix from matching points using 
linear least squares eight point algorithm
Arguments:
    points1 - N points in the first image that match with points2
    points2 - N points in the second image that match with points1

    Both points1 and points2 are from the get_data_from_txt_file() method
Returns:
    F - the fundamental matrix such that (points2)^T * F * points1 = 0
Please see lecture notes and slides to see how the linear least squares eight
point algorithm works
'''
def lls_eight_point_alg(points1, points2):

    numPoints = points1.shape[0]
    if numPoints != points2.shape[0]:
        raise Exception('Error points1 and points2 are not same size')

    A = np.zeros((numPoints, 9))

    for i in range(numPoints):
        p1 = np.reshape(points1[i], (1, 3))
        p2 = np.reshape(points2[i], (3, 1))
        A[i, :] = p2.dot(p1).flatten()

    U, s, VT = np.linalg.svd(A)
    F_hat = VT[-1, :]
    # force rank 2
    F_hat = np.reshape(F_hat, (3, 3))
    U, eigen, VT = np.linalg.svd(F_hat, full_matrices=True)

    s = np.zeros((3, 3))
    s[0][0] = eigen[0]
    s[1][1] = eigen[1]

    F = np.dot(U, np.dot(s, VT))

    return F

'''
NORMALIZED_EIGHT_POINT_ALG  computes the fundamental matrix from matching points
using the normalized eight point algorithm
Arguments:
    points1 - N points in the first image that match with points2
    points2 - N points in the second image that match with points1

    Both points1 and points2 are from the get_data_from_txt_file() method
Returns:
    F - the fundamental matrix such that (points2)^T * F * points1 = 0
Please see lecture notes and slides to see how the normalized eight
point algorithm works
'''
def normalized_eight_point_alg(points1, points2):
    # TODO: Implement this method!

    numPoints = points1.shape[0]
    if numPoints != points2.shape[0]:
        raise Exception('Error points1 and points2 are not same size')

    center1 = np.sum(points1, axis = 0) / numPoints
    center2 = np.sum(points2, axis = 0) / numPoints

    p1 = points1 - center1
    p2 = points2 - center2

    # scale
    p1_2 = p1 ** 2
    scale1 = np.sqrt(4 / np.sum(p1_2[:, 0] + p1_2[:, 1]) * numPoints)

    p2_2 = p2 ** 2
    scale2 = np.sqrt(4 / np.sum(p2_2[:, 0] + p2_2[:, 1]) * numPoints)

    T1 = np.array([
                    [scale1, 0, -scale1 * center1[0]],
                    [0, scale1, -scale1 * center1[1]],
                    [0, 0, 1]])

    T2 = np.array([
                    [scale2, 0, -scale2 * center2[0]],
                    [0, scale2, -scale2 * center2[1]],
                    [0, 0, 1]])

    p1 = T1.dot(points1.T).T
    p2 = T2.dot(points2.T).T

    Fq = lls_eight_point_alg(p1, p2)

    F = T2.T.dot(Fq).dot(T1)

    return F

'''
PLOT_EPIPOLAR_LINES_ON_IMAGES given a pair of images and corresponding points,
draws the epipolar lines on the images
Arguments:
    points1 - N points in the first image that match with points2
    points2 - N points in the second image that match with points1
    im1 - a HxW(xC) matrix that contains pixel values from the first image 
    im2 - a HxW(xC) matrix that contains pixel values from the second image 
    F - the fundamental matrix such that (points2)^T * F * points1 = 0

    Both points1 and points2 are from the get_data_from_txt_file() method
Returns:
    Nothing; instead, plots the two images with the matching points and
    their corresponding epipolar lines. See Figure 1 within the problem set
    handout for an example
'''
def plot_epipolar_lines_on_images(points1, points2, im1, im2, F):
    # TODO: Implement this method!

    # plot lines on first image
    plt.subplot(1,2,1)
    ln1 = F.T.dot(points2.T)
    for i in xrange(ln1.shape[1]):
        plt.plot([0, im1.shape[1]], [-ln1[2][i]*1.0/ln1[1][i], -(ln1[2][i]+ln1[0][i]*im1.shape[1])*1.0/ln1[1][i]], 'r')
        plt.plot([points1[i][0]], [points1[i][1]], 'b*')
    plt.imshow(im1, cmap='gray')

    plt.subplot(1,2,2)
    ln2 = F.dot(points1.T)
    for i in xrange(ln2.shape[1]):
        plt.plot([0, im2.shape[1]], [-ln2[2][i]*1.0/ln2[1][i], -(ln2[2][i]+ln2[0][i]*im2.shape[1])/ln2[1][i]], 'r')
        plt.plot([points2[i][0]], [points2[i][1]], 'b*')
    plt.imshow(im2, cmap='gray')
    #raise Exception('Not Implemented Error')

'''
COMPUTE_DISTANCE_TO_EPIPOLAR_LINES  computes the average distance of a set a 
points to their corresponding epipolar lines
Arguments:
    points1 - N points in the first image that match with points2
    points2 - N points in the second image that match with points1
    F - the fundamental matrix such that (points2)^T * F * points1 = 0

    Both points1 and points2 are from the get_data_from_txt_file() method
Returns:
    average_distance - the average distance of each point to the epipolar line
'''
def compute_distance_to_epipolar_lines(points1, points2, F):
    # TODO: Implement this method!


    line1 = F.T.dot(points2.T)
    numPoints = points1.shape[0]

    top = np.sum(points1 * (line1.T), axis = 1)

    F_F = (line1 ** 2)
    bot = np.sqrt(F_F[0, :] + F_F[1, :])

    distance = np.sum(np.abs(top) / bot) / numPoints
    return distance

if __name__ == '__main__':
    for im_set in ['data/set1', 'data/set2']:
        print '-'*80
        print "Set:", im_set
        print '-'*80

        # Read in the data
        im1 = imread(im_set+'/image1.jpg')
        im2 = imread(im_set+'/image2.jpg')
        points1 = get_data_from_txt_file(im_set+'/pt_2D_1.txt')
        points2 = get_data_from_txt_file(im_set+'/pt_2D_2.txt')
        assert (points1.shape == points2.shape)

        # Running the linear least squares eight point algorithm
        F_lls = lls_eight_point_alg(points1, points2)
        print "Fundamental Matrix from LLS  8-point algorithm:\n", F_lls
        print "Distance to lines in image 1 for LLS:", \
            compute_distance_to_epipolar_lines(points1, points2, F_lls)
        print "Distance to lines in image 2 for LLS:", \
            compute_distance_to_epipolar_lines(points2, points1, F_lls.T)

        # Running the normalized eight point algorithm
        F_normalized = normalized_eight_point_alg(points1, points2)

        pFp = [points2[i].dot(F_normalized.dot(points1[i])) 
            for i in xrange(points1.shape[0])]
        print "p'^T F p =", np.abs(pFp).max()
        print "Fundamental Matrix from normalized 8-point algorithm:\n", \
            F_normalized
        print "Distance to lines in image 1 for normalized:", \
            compute_distance_to_epipolar_lines(points1, points2, F_normalized)
        print "Distance to lines in image 2 for normalized:", \
            compute_distance_to_epipolar_lines(points2, points1, F_normalized.T)

        # Plotting the epipolar lines
        plt.figure("Without Normalization")
        plot_epipolar_lines_on_images(points1, points2, im1, im2, F_lls)
        plt.figure("Normalized")
        plot_epipolar_lines_on_images(points1, points2, im1, im2, F_normalized)
        plt.show()
