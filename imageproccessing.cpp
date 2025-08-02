#include "imageproccessing.hpp"
#include "kernels.hpp"
#include <QDebug>
#include <cmath>

ImageProccessing::ImageProccessing() {}

#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <algorithm>

using namespace cv;
using namespace std;

/**
 * @brief Faire pivoter une image d'un angle spécifié (0, 90, 180, 270, 360).
 * 
 * Cette fonction effectue une rotation de l'image d'entrée selon l'angle donné. Elle supporte des angles
 * spécifiques comme 0, 90, 180, 270 et 360 degrés. Les autres angles sont traités en retournant l'image d'origine.
 * 
 * @param inputImage L'image à faire pivoter.
 * @param angle L'angle de rotation (en degrés).
 * 
 * @return L'image pivotée de type Mat.
 */
Mat ImageProccessing::rotateImage(const Mat& inputImage, int angle) {
    
    Mat rotatedImage;

    // Gérer les angles de rotation spécifiques (0, 90, 180, 360)
    switch (angle) {
        case 90:
            // Rotation de 90 degrés dans le sens horaire
            transpose(inputImage, rotatedImage); // Transposer (échanger les lignes et les colonnes)
            flip(rotatedImage, rotatedImage, 1);  // Retourner horizontalement
            break;
        case 180:
            // Rotation de 180 degrés
            flip(inputImage, rotatedImage, -1);  // Retourner à la fois horizontalement et verticalement
            break;
        case 270:
            // Rotation de 270 degrés (identique à -90 degrés)
            transpose(inputImage, rotatedImage); // Transposer (échanger les lignes et les colonnes)
            flip(rotatedImage, rotatedImage, 0);  // Retourner verticalement
            break;
        case 360:
            // Rotation de 360 degrés (aucun changement)
            rotatedImage = inputImage.clone();
            break;
        case 0:
        default:
            // Si 0 degrés ou tout autre angle, retourner simplement l'image d'origine
            rotatedImage = inputImage.clone();
            break;
    }

    return rotatedImage;
}

/**
 * @brief Applique une convolution sur une image en utilisant un noyau (kernel).
 * 
 * Cette fonction effectue une convolution entre une image et un noyau (kernel). Chaque pixel de l'image
 * est mis à jour en fonction de la somme pondérée des pixels voisins, selon les valeurs du noyau.
 * Le résultat est stocké dans une nouvelle image.
 *
 * @param kernel Le noyau de convolution, une matrice 2D (vector de vector de float).
 * @param image L'image d'entrée sur laquelle la convolution est appliquée.
 * 
 * @return Mat L'image de sortie après convolution, de type CV_64F.
 * 
 * @throws runtime_error Si l'image d'entrée est vide.
 * 
 * @note Les valeurs du noyau sont appliquées sur chaque pixel de l'image, en prenant en compte les pixels voisins
 *       (en utilisant des indices valides à l'intérieur de l'image). Les bords de l'image sont traités en
 *       ignorant les valeurs du noyau qui sortiraient de l'image.
 */
Mat convolution(const vector<vector<float>>& kernel, const Mat& image) {
    if (image.empty()) {
        throw runtime_error("L'image d'entrée est vide.");
    }
    
    int kernelWidth = kernel[0].size();
    int kernelHeight = kernel.size();
    int offsetX = kernelWidth / 2;
    int offsetY = kernelHeight / 2;

    // Image de sortie
    Mat output(image.size(), CV_64F);

    // Convolution
    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            float sum = 0.0;

            // Appliquer le noyau (kernel) autour du pixel
            for (int j = 0; j < kernelHeight; ++j) {
                for (int i = 0; i < kernelWidth; ++i) {
                    int imageX = x + i - offsetX;
                    int imageY = y + j - offsetY;

                    // Vérifier si le pixel est à l'intérieur de l'image
                    if (imageX >= 0 && imageX < image.cols && imageY >= 0 && imageY < image.rows) {
                        sum += kernel[j][i] * image.at<uchar>(imageY, imageX);
                    }
                }
            }

            // Limiter la valeur à [0, 255]
            output.at<float>(y, x) = sum;
        }
    }

    return output;
}

/**
 * @brief Génère un noyau gaussien de taille et de sigma spécifiés.
 * 
 * Cette fonction génère un noyau unidimensionnel de type gaussien. Le noyau est utilisé pour des opérations
 * de filtrage, comme le flou gaussien, et peut être utilisé dans des algorithmes de traitement d'images.
 * Si certaines conditions sont remplies, un noyau gaussien fixe est utilisé à la place du calcul.
 *
 * @param n La taille du noyau gaussien (doit être impair).
 * @param sigma L'écart-type de la fonction gaussienne. Si ce paramètre est négatif ou nul, une valeur par défaut
 *              est calculée en fonction de `n`.
 * @param ktype Le type des éléments du noyau, qui peut être soit `CV_32F` (32 bits flottants) ou `CV_64F` (64 bits flottants).
 * 
 * @return Mat Le noyau gaussien généré, de taille `n` x 1, avec le type spécifié par `ktype`.
 * 
 * @throws cv::Exception Si `ktype` n'est pas l'un des types valides (`CV_32F` ou `CV_64F`).
 * 
 * @note Si `n` est inférieur ou égal à une certaine taille (7) et que `sigma` est nul ou négatif,
 *       un noyau pré-calculé est utilisé pour optimiser les performances.
 * 
 * @note Le noyau est normalisé de manière à ce que la somme des éléments soit égale à 1.
 */
Mat getGaussianKernel(int n, double sigma, int ktype) {
    const int SMALL_GAUSSIAN_SIZE = 7;
    static const float small_gaussian_tab[][SMALL_GAUSSIAN_SIZE] = {
        {1.f},
        {0.25f, 0.5f, 0.25f},
        {0.0625f, 0.25f, 0.375f, 0.25f, 0.0625f},
        {0.03125f, 0.109375f, 0.21875f, 0.28125f, 0.21875f, 0.109375f, 0.03125f}
    };

    // Sélectionner un noyau fixe si les conditions sont remplies
    const float* fixed_kernel = n % 2 == 1 && n <= SMALL_GAUSSIAN_SIZE && sigma <= 0 ?
                                    small_gaussian_tab[n >> 1] : 0;

    CV_Assert(ktype == CV_32F || ktype == CV_64F);
    Mat kernel(n, 1, ktype);
    float* cf = (float*)kernel.data;
    double* cd = (double*)kernel.data;

    double sigmaX = sigma > 0 ? sigma : ((n - 1) * 0.5 - 1) * 0.3 + 0.8;
    double scale2X = -0.5 / (sigmaX * sigmaX);
    double sum = 0;

    // Calculer les valeurs du noyau
    for (int i = 0; i < n; i++) {
        double x = i - (n - 1) * 0.5;
        double t = fixed_kernel ? (double)fixed_kernel[i] : exp(scale2X * x * x);
        if (ktype == CV_32F) {
            cf[i] = (float)t;
            sum += cf[i];
        } else {
            cd[i] = t;
            sum += cd[i];
        }
    }

    // Normaliser le noyau
    sum = 1. / sum;
    for (int i = 0; i < n; i++) {
        if (ktype == CV_32F)
            cf[i] = (float)(cf[i] * sum);
        else
            cd[i] *= sum;
    }

    return kernel;
}

/**
 * @brief Applique un flou gaussien à une image en utilisant un noyau gaussien.
 * 
 * Cette fonction effectue un flou gaussien sur l'image d'entrée en utilisant un noyau gaussien à deux dimensions.
 * Le noyau est soit spécifié par la taille `ksize`, soit calculé automatiquement en fonction de la valeur de `sigma1` et `sigma2`.
 * La fonction applique la convolution gaussienne séparée dans les directions horizontale et verticale.
 *
 * @param src L'image d'entrée, de type `Mat`.
 * @param dst L'image de sortie, de type `Mat`. Elle contient l'image après application du flou gaussien.
 * @param ksize La taille du noyau de convolution. Si l'une des dimensions est inférieure ou égale à zéro, elle sera calculée automatiquement en fonction de `sigma1` et `sigma2`.
 * @param sigma1 L'écart-type de la distribution gaussienne dans la direction horizontale. Si `sigma2` est inférieur ou égal à zéro, cette valeur est utilisée pour les deux directions.
 * @param sigma2 L'écart-type de la distribution gaussienne dans la direction verticale. Si cette valeur est inférieure ou égale à zéro, `sigma1` est utilisée pour les deux directions.
 * @param borderType Le type de bordure à utiliser pour gérer les bords de l'image (par exemple, `BORDER_DEFAULT`, `BORDER_REFLECT`, etc.).
 * 
 * @throws cv::Exception Si les tailles du noyau (`ksize.width` et `ksize.height`) ne sont pas impaires ou si elles sont invalides.
 * 
 * @note Si `ksize` est défini sur `(1, 1)`, l'image source est copiée directement dans l'image de destination sans appliquer de flou.
 *       Si `ksize.width` ou `ksize.height` est inférieur ou égal à zéro, la taille du noyau est automatiquement calculée en fonction de `sigma1` et `sigma2`.
 *       La fonction applique un filtrage séparé dans les directions horizontale et verticale en utilisant des noyaux gaussiens calculés avec `cv::getGaussianKernel`.
 */
void GaussianBlur(const Mat& src, Mat& dst, Size ksize, double sigma1, double sigma2, int borderType) {
    if (ksize.width == 1 && ksize.height == 1) {
        src.copyTo(dst);
        return;
    }

    int depth = src.depth();
    if (sigma2 <= 0)
        sigma2 = sigma1;

    // Détection automatique de la taille du noyau à partir de sigma
    if (ksize.width <= 0 && sigma1 > 0)
        ksize.width = cvRound(sigma1 * (depth == CV_8U ? 3 : 4) * 2 + 1) | 1;
    if (ksize.height <= 0 && sigma2 > 0)
        ksize.height = cvRound(sigma2 * (depth == CV_8U ? 3 : 4) * 2 + 1) | 1;

    CV_Assert(ksize.width > 0 && ksize.width % 2 == 1 &&
              ksize.height > 0 && ksize.height % 2 == 1);

    sigma1 = max(sigma1, 0.);
    sigma2 = max(sigma2, 0.);

    // Obtenir les noyaux gaussiens
    Mat kx = cv::getGaussianKernel(ksize.width, sigma1, CV_32F);
    Mat ky;
    if (ksize.height == ksize.width && abs(sigma1 - sigma2) < DBL_EPSILON)
        ky = kx;
    else
        ky = cv::getGaussianKernel(ksize.height, sigma2, CV_32F);

    // Appliquer la convolution séparée
    Mat temp;
    sepFilter2D(src, temp, -1, kx, ky, cv::Point(-1, -1), 0, borderType);
    dst = temp;
}

/**
 * @brief Convertit une image en niveaux de gris.
 * 
 * Cette fonction effectue la conversion d'une image en niveaux de gris en fonction du nombre de canaux de l'image d'entrée.
 * Si l'image a 4 canaux (BGRA), l'alpha est ignoré. Si l'image a 2 canaux, elle est d'abord dupliquée pour créer une image à 3 canaux.
 * Si l'image est déjà en niveaux de gris (1 canal), elle est renvoyée sans modification.
 *
 * @param inputImage L'image d'entrée à convertir en niveaux de gris.
 * 
 * @return Mat L'image convertie en niveaux de gris, de type `CV_8U` avec un seul canal.
 * 
 * @throws runtime_error Si le nombre de canaux de l'image n'est pas pris en charge pour la conversion.
 * 
 * @note Cette fonction prend en charge les images ayant 1, 2, 3 ou 4 canaux. Si l'image a 2 canaux, ceux-ci sont fusionnés
 *       pour créer une image à 3 canaux avant la conversion en niveaux de gris.
 * 
 * @warning Cette fonction suppose que l'image d'entrée est dans l'un des formats suivants :
 *          - 4 canaux (BGRA) : L'alpha est ignoré.
 *          - 3 canaux (BGR) : Conversion directe en niveaux de gris.
 *          - 2 canaux : Fusion des deux canaux en une image à 3 canaux avant conversion.
 *          - 1 canal (Grayscale) : Aucune conversion nécessaire.
 */
Mat ImageProccessing::toGrayScale(const Mat& inputImage) {

    qDebug() << "Début de la conversion en niveaux de gris.";
    qDebug() << "Taille de l'image d'entrée:" << inputImage.cols << "x" << inputImage.rows;
    qDebug() << "Nombre de canaux de l'image d'entrée:" << inputImage.channels();
    qDebug() << "Type de l'image d'entrée:" << inputImage.type();

    Mat grayImage;

    if (inputImage.channels() == 4) {
        qDebug() << "Conversion de BGRA en BGR en supprimant le canal alpha.";
        Mat bgrImage;
        cvtColor(inputImage, bgrImage, COLOR_BGRA2BGR);
        qDebug() << "Conversion de BGR en niveaux de gris.";
        cvtColor(bgrImage, grayImage, COLOR_BGR2GRAY);
    } else if (inputImage.channels() == 3) {
        qDebug() << "Conversion de BGR en niveaux de gris.";
        cvtColor(inputImage, grayImage, COLOR_BGR2GRAY);
    } else if (inputImage.channels() == 2) {
        qDebug() << "Conversion d'une image à 2 canaux en niveaux de gris.";
    
        Mat mergedChannels;
        merge(vector<Mat>{inputImage, inputImage}, mergedChannels);  // Exemple : dupliquer pour créer une image 3 canaux
        cvtColor(mergedChannels, grayImage, COLOR_BGR2GRAY);
    } else if (inputImage.channels() == 1) {
        qDebug() << "L'image est déjà en niveaux de gris.";
        grayImage = inputImage.clone();
    } else {
        throw runtime_error("Nombre de canaux non supporté pour la conversion en niveaux de gris.");
    }

    qDebug() << "Valeur du pixel à (0, 0) dans l'image en niveaux de gris:" << grayImage.at<uchar>(0, 0);

    qDebug() << "Conversion en niveaux de gris terminée avec succès.";

    return grayImage;
}

/**
 * @brief Applique un filtre médian personnalisé sur une image (grayscale ou couleur).
 * 
 * Cette fonction applique un filtre médian sur l'image d'entrée. Le filtre médian remplace chaque pixel par la médiane
 * des pixels voisins dans une fenêtre de taille `kernelSize`. Si l'image est en niveaux de gris, le filtre est appliqué
 * directement sur l'image. Si l'image est en couleur, le filtre est appliqué séparément sur chaque canal (R, G, B).
 * 
 * @param inputImage L'image d'entrée à laquelle le filtre médian sera appliqué. Elle peut être en niveaux de gris ou en couleur.
 * @param kernelSize La taille du noyau du filtre médian. Ce paramètre doit être un nombre impair et supérieur ou égal à 3.
 * 
 * @return Mat L'image filtrée après application du filtre médian.
 * 
 * @throws std::invalid_argument Si `kernelSize` est un nombre pair ou inférieur à 3.
 * @throws std::runtime_error Si l'image d'entrée est vide.
 * 
 * @note Le filtre médian est appliqué séparément pour chaque pixel en fonction de ses voisins dans un voisinage défini par `kernelSize`.
 *       Les bords de l'image sont gérés en utilisant la fonction `clamp` pour éviter l'accès à des pixels en dehors de l'image.
 * 
 * @warning Cette fonction fonctionne à la fois pour des images en niveaux de gris (1 canal) et en couleur (3 canaux).
 */
Mat ImageProccessing::applyCustomMedianFilter(const Mat& inputImage, int kernelSize) {

    if (kernelSize % 2 == 0 || kernelSize < 3) {
        throw invalid_argument("La taille du noyau doit être un nombre impair et >= 3");
    }

      // Vérifier si l'image est vide
    if (inputImage.empty()) {
        throw runtime_error("L'image d'entrée est vide. Impossible d'appliquer le filtre.");
    }

    if (inputImage.channels() == 1) {
        // Initialiser l'image de sortie avec la même taille et le même type
        Mat filteredImage = Mat::zeros(inputImage.size(), CV_8UC1);

        int halfKernel = kernelSize / 2;

        // Appliquer le filtre médian
        for (int y = 0; y < inputImage.rows; ++y) {
            for (int x = 0; x < inputImage.cols; ++x) {
                vector<uchar> neighborhood;

                for (int dy = -halfKernel; dy <= halfKernel; ++dy) {
                    for (int dx = -halfKernel; dx <= halfKernel; ++dx) {
                        int ny = clamp(y + dy, 0, inputImage.rows - 1);
                        int nx = clamp(x + dx, 0, inputImage.cols - 1);
                        neighborhood.push_back(inputImage.at<uchar>(ny, nx));
                    }
                }

                // Trier les valeurs du voisinage et sélectionner la médiane
                sort(neighborhood.begin(), neighborhood.end());
                filteredImage.at<uchar>(y, x) = neighborhood[neighborhood.size() / 2];
            }
        }

        return filteredImage;
    }

    // Initialiser les canaux de sortie
    vector<Mat> channels;
    split(inputImage, channels); // Diviser l'image couleur en 3 canaux (B, G, R)

    // Appliquer le filtre médian personnalisé sur chaque canal
    for (size_t i = 0; i < channels.size(); ++i) {
        Mat filteredChannel = Mat::zeros(channels[i].size(), CV_8UC1);

        int halfKernel = kernelSize / 2;
        for (int y = 0; y < channels[i].rows; ++y) {
            for (int x = 0; x < channels[i].cols; ++x) {
                vector<uchar> neighborhood;

                for (int dy = -halfKernel; dy <= halfKernel; ++dy) {
                    for (int dx = -halfKernel; dx <= halfKernel; ++dx) {
                        int ny = clamp(y + dy, 0, channels[i].rows - 1);
                        int nx = clamp(x + dx, 0, channels[i].cols - 1);
                        neighborhood.push_back(channels[i].at<uchar>(ny, nx));
                    }
                }

                sort(neighborhood.begin(), neighborhood.end());
                filteredChannel.at<uchar>(y, x) = neighborhood[neighborhood.size() / 2];
            }
        }

        // Remplacer le canal par le résultat filtré
        channels[i] = filteredChannel.clone();
    }

    // Combiner les canaux filtrés pour recréer l'image couleur
    Mat filteredImage;
    merge(channels, filteredImage);

    return filteredImage;
}

/**
 * @brief Applique la détection des contours en utilisant l'opérateur Sobel.
 * 
 * Cette fonction utilise les noyaux de Sobel pour détecter les contours dans une image en calculant les gradients dans les directions horizontale et verticale. Ensuite, elle combine ces gradients pour obtenir la magnitude des contours, résultant en une image binaire représentant les bords détectés.
 * 
 * @param image L'image d'entrée en niveaux de gris (de type `CV_8UC1`) sur laquelle la détection des contours sera effectuée.
 * @return Mat L'image résultante contenant les contours détectés. Elle est de même taille que l'image d'entrée et est de type `CV_8UC1`.
 * 
 * @note L'opérateur Sobel est utilisé pour calculer les dérivées de l'intensité dans les directions horizontale et verticale, puis ces dérivées sont combinées pour calculer la magnitude des gradients.
 * @note L'image résultante est une image en niveaux de gris où les pixels blancs (valeur proche de 255) représentent les contours détectés et les pixels noirs (valeur proche de 0) représentent les zones sans contours.
 * 
 * @warning Cette fonction suppose que l'image d'entrée est en niveaux de gris (1 canal). Si l'image est en couleur ou a plus de 1 canal, un prétraitement est nécessaire.
 */
Mat ImageProccessing::applyEdgeDetection(const Mat& Inputimage) {
    
    if (Inputimage.empty()) {
        throw runtime_error("L'image d'entrée est vide. Impossible d'appliquer le traitement.");
    }

    Mat grayImage = toGrayScale(Inputimage);

    qDebug() << "Type de l'image après conversion en niveaux de gris:" << grayImage.type();
    qDebug() << "Taille de l'image: largeur =" << grayImage.size().width << ", hauteur =" << grayImage.size().height;

    // Définir les noyaux de Sobel
    vector<vector<float>> sobelX = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    vector<vector<float>> sobelY = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };

    // Calculer les gradients en X et Y
    Mat gradX = convolution(sobelX, grayImage);
    Mat gradY = convolution(sobelY, grayImage);

    // Calculer la magnitude du gradient
    Mat edges = Mat::zeros(grayImage.size(), CV_8UC1);
    for (int y = 0; y < grayImage.rows; ++y) {
        for (int x = 0; x < grayImage.cols; ++x) {
            float gx = gradX.at<float>(y, x); // Accéder en tant que float
            float gy = gradY.at<float>(y, x); // Accéder en tant que float
            float magnitude = sqrt(gx * gx + gy * gy);

            // Normaliser les valeurs entre [0, 255]
            edges.at<uchar>(y, x) = static_cast<uchar>(clamp(magnitude, 0.0f, 255.0f));
        }
    }

    qDebug() << "Détection des contours terminée avec succès.";

    return edges;
}

/**
 * @brief Applique un seuillage simple sur une image en niveaux de gris.
 *
 * Cette fonction prend une image en entrée, la convertit en niveaux de gris si elle est en couleur,
 * puis applique un seuillage simple pour créer une image binaire. Tous les pixels ayant une intensité
 * supérieure à la valeur seuil sont définis comme blancs (255), tandis que les autres sont définis comme noirs (0).
 * 
 * @param inputImage L'image d'entrée (peut être en couleur ou en niveaux de gris).
 * 
 * @return Mat L'image binaire résultante après application du seuillage.
 *
 * @note Si l'image d'entrée est en couleur, elle est convertie en niveaux de gris avant d'appliquer le seuillage.
 *       La valeur de seuil est fixée à 128 dans cette implémentation.
 */
Mat ImageProccessing::applyThreshold(const Mat& inputImage, int thresholdValue) {
   
    // Convertir l'image en niveaux de gris si elle est en couleur
    Mat grayImage;

    if (inputImage.channels() > 1) {
        grayImage = toGrayScale(inputImage);
    } else {
        grayImage = inputImage;
    }

    // Créer une image binaire pour stocker le résultat du seuillage
    Mat binaryImage = Mat::zeros(grayImage.size(), CV_8UC1);

    // Appliquer le seuillage
    for (int y = 0; y < grayImage.rows; y++) {
        for (int x = 0; x < grayImage.cols; x++) {
            // Obtenir l'intensité du pixel
            uchar pixelValue = grayImage.at<uchar>(y, x);

            // Appliquer le seuil
            if (pixelValue > thresholdValue) {
                binaryImage.at<uchar>(y, x) = 255; // Pixel blanc (objet)
            } else {
                binaryImage.at<uchar>(y, x) = 0;   // Pixel noir (fond)
            }
        }
    }

    qDebug() << "Seuillage terminé avec succès.";

    return binaryImage;
}

/**
 * @brief Calcule l'histogramme d'une image et retourne l'image de l'histogramme.
 * 
 * Cette fonction effectue les étapes suivantes :
 * - Conversion de l'image en niveaux de gris si l'image est en couleur (RGB).
 * - Calcul de l'histogramme des intensités de pixels.
 * - Normalisation de l'histogramme pour le redimensionner à une taille d'image spécifiée.
 * - Dessin de l'histogramme normalisé sur une image de fond blanc.
 * 
 * @param inputImage L'image d'entrée pour laquelle l'histogramme doit être calculé. Elle peut être en couleur ou en niveaux de gris.
 * @return Mat L'image représentant l'histogramme normalisé de l'image d'entrée.
 * 
 * @note Cette fonction fonctionne uniquement avec des images en niveaux de gris ou en couleur (3 canaux). 
 * Si l'image est en couleur, elle sera convertie en niveaux de gris avant le calcul de l'histogramme.
 * 
 * @warning L'image d'entrée doit être une image valide, sinon un comportement indéfini pourrait se produire.
 */
Mat ImageProccessing::calculateHistogram(const Mat& inputImage) {
    // Étape 1 : Conversion en niveaux de gris si nécessaire
    Mat grayImage;
    if (inputImage.channels() > 1) {
        cvtColor(inputImage, grayImage, COLOR_BGR2GRAY);
    } else {
        grayImage = inputImage.clone();
    }

    // Étape 2 : Initialiser un tableau pour l'histogramme (256 bins)
    int histSize = 256;
    vector<int> histogram(histSize, 0);

    // Étape 3 : Parcourir l'image pour remplir l'histogramme
    for (int y = 0; y < grayImage.rows; y++) {
        for (int x = 0; x < grayImage.cols; x++) {
            int pixelValue = grayImage.at<uchar>(y, x);
            histogram[pixelValue]++;
        }
    }

    // Étape 4 : Normalisation de l'histogramme pour le redimensionner à l'image
    int maxVal = *max_element(histogram.begin(), histogram.end());

    if (maxVal == 0) {
        return Mat(); // Retourner une image vide en cas de problème
    }

    int histWidth = 512, histHeight = 400;
    Mat histImage(histHeight + 50, histWidth + 50, CV_8UC3, Scalar(255, 255, 255)); // Fond blanc

    // Normalisation pour que les valeurs soient entre 0 et histHeight
    for (int i = 0; i < histSize; i++) {
        histogram[i] = ((double)histogram[i] / maxVal) * histHeight;
    }

    // Étape 5 : Dessin de l'histogramme
    int binWidth = cvRound((double)histWidth / histSize);
    Scalar barColor = Scalar(50, 50, 150); // Bleu foncé pour les barres
    for (int i = 0; i < histSize; i++) {
        rectangle(histImage,
                  Point(25 + binWidth * i, histHeight + 25),
                  Point(25 + binWidth * (i + 1), histHeight + 25 - histogram[i]),
                  barColor, FILLED);
    }

    // Ajouter les axes
    line(histImage, Point(25, 25), Point(25, histHeight + 25), Scalar(0, 0, 0), 2); // Axe vertical
    line(histImage, Point(25, histHeight + 25), Point(histWidth + 25, histHeight + 25), Scalar(0, 0, 0), 2); // Axe horizontal

    // Ajouter des annotations pour les axes
    putText(histImage, "Intensite", Point(histWidth / 2, histHeight + 45), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 0), 1);
    putText(histImage, "0", Point(20, histHeight + 30), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);
    putText(histImage, "255", Point(histWidth, histHeight + 30), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);

    // Ajouter des valeurs sur l'axe des ordonnées (par exemple : 0, max/2, max)
    putText(histImage, "0", Point(5, histHeight + 25), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);
    putText(histImage, to_string(maxVal / 2), Point(5, (histHeight + 25) / 2), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);
    putText(histImage, to_string(maxVal), Point(5, 25), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);

    return histImage;
}

/**
 * @brief Applique l'algorithme SIFT (Scale-Invariant Feature Transform) pour détecter des points-clés 
 *        et descripteurs d'une image.
 * 
 * Cette fonction implémente SIFT, un algorithme robuste utilisé pour détecter des caractéristiques
 * distinctives dans une image, même sous des variations d'échelle, de rotation et d'illumination.
 * 
 * ### Fonctionnement :
 * - Convertit l'image en niveaux de gris (si nécessaire) pour simplifier le traitement.
 * - Détecte des points-clés dans l'image, qui correspondent à des régions d'intérêt importantes.
 * - Génère des descripteurs pour chaque point-clé, représentant les caractéristiques locales.
 * - Superpose les points-clés détectés sur l'image d'entrée pour visualiser les résultats.
 * 
 * ### Utilisations :
 * - Correspondance d'images (image matching).
 * - Suivi d'objets (object tracking).
 * - Reconstruction 3D basée sur des images.
 * - Classification et reconnaissance d'objets.
 * 
 * @param inputImage L'image d'entrée pour laquelle les points-clés et descripteurs seront calculés.
 *                   Elle peut être en couleur ou en niveaux de gris.
 * @return cv::Mat Une copie de l'image d'entrée avec les points-clés superposés en vert.
 * @throws std::invalid_argument Si l'image d'entrée est vide.
 * 
 * @note Cette implémentation utilise la classe `cv::SIFT` fournie par OpenCV.
 * 
 * ### Exemple d'utilisation :
 * @code
 * cv::Mat image = cv::imread("exemple.jpg"); // Charger une image
 * ImageProcessing processor;
 * cv::Mat resultat = processor.appliquerSIFT(image);
 * cv::imshow("Résultat SIFT", resultat); // Afficher l'image avec les points-clés
 * cv::waitKey(0);
 * @endcode
 */
Mat ImageProccessing::applySIFT(const Mat& inputImage) {
    // Conversion en niveaux de gris (si nécessaire)
    Mat imageGris;
    if (inputImage.channels() == 3)
        cvtColor(inputImage, imageGris, COLOR_BGR2GRAY);
    else
        imageGris = inputImage;

    // Détecter les points-clés et les descripteurs avec SIFT
    Ptr<SIFT> sift = SIFT::create();
    vector<KeyPoint> pointsCles;
    Mat descripteurs;

    sift->detectAndCompute(imageGris, noArray(), pointsCles, descripteurs);

    // Dessiner les points-clés sur l'image
    Mat imageResultat;
    drawKeypoints(inputImage, pointsCles, imageResultat, Scalar(0, 255, 0));

    return imageResultat;
}

/**
 * @brief Applique l'opération d'érosion sur une image en niveaux de gris.
 * 
 * L'érosion est une opération morphologique qui remplace chaque pixel par la valeur minimale de ses voisins
 * dans un voisinage défini par un noyau de taille `kernelSize`. Cette fonction applique l'érosion à une image
 * en niveaux de gris. Si l'image d'entrée est en couleur, elle est d'abord convertie en niveaux de gris.
 * 
 * @param inputImage L'image d'entrée sur laquelle l'érosion sera appliquée. Elle peut être en couleur ou en niveaux de gris.
 * @param kernelSize La taille du noyau d'érosion. Ce paramètre doit être un entier impair et supérieur à zéro.
 * 
 * @return Mat L'image résultante après application de l'érosion.
 * 
 * @throws std::invalid_argument Si `kernelSize` n'est pas un entier positif impair.
 * 
 * @note L'érosion est effectuée en parcourant chaque pixel de l'image et en remplaçant sa valeur par la valeur
 *       minimale de ses voisins dans un voisinage de taille `kernelSize`. Les bords de l'image sont traités
 *       en ajoutant des bordures réfléchissantes pour éviter les artefacts.
 * 
 * @warning Cette fonction suppose que l'image est en niveaux de gris ou qu'elle peut être convertie en niveaux de gris.
 */
Mat ImageProccessing::applyErosion(const Mat& inputImage, int kernelSize) {
    // Vérification : la taille du noyau doit être impaire et supérieure à zéro
    if (kernelSize <= 0 || kernelSize % 2 == 0) {
        throw invalid_argument("La taille du noyau doit être un entier positif impair.");
    }

    // Vérifier si l'image est en niveaux de gris
    Mat grayImage;
    if (inputImage.channels() > 1) {
        cvtColor(inputImage, grayImage, COLOR_BGR2GRAY);
    } else {
        grayImage = inputImage.clone();
    }

    // Ajouter des bordures pour éviter les artefacts sur les bords
    int offset = kernelSize / 2;
    Mat paddedImage;
    copyMakeBorder(grayImage, paddedImage, offset, offset, offset, offset, BORDER_REFLECT);

    // Créer une image de sortie
    Mat outputImage = Mat::zeros(grayImage.size(), CV_8U);

    // Appliquer l'érosion manuelle
    for (int i = offset; i < paddedImage.rows - offset; i++) {
        for (int j = offset; j < paddedImage.cols - offset; j++) {
            int minVal = 255;

            // Parcourir le noyau structurant
            for (int ki = -offset; ki <= offset; ki++) {
                for (int kj = -offset; kj <= offset; kj++) {
                    minVal = min(minVal, (int)paddedImage.at<uchar>(i + ki, j + kj));
                }
            }

            outputImage.at<uchar>(i - offset, j - offset) = minVal;
        }
    }

    return outputImage;
}

Mat hardcodedGaussianKernel() {
    return (Mat_<float>(3, 3) <<
                0.0751136, 0.123841, 0.0751136,
            0.123841,  0.204180, 0.123841,
            0.0751136, 0.123841, 0.0751136
            );
}

/**
 * @brief Applique un flou gaussien sur une image à un seul canal à l'aide d'un noyau de convolution.
 * 
 * Cette fonction applique un flou gaussien sur une image en utilisant un noyau de convolution spécifié. 
 * Le flou gaussien est une opération de filtrage qui remplace chaque pixel de l'image par une moyenne pondérée 
 * de ses voisins, avec des poids donnés par le noyau gaussien. La fonction est appliquée sur une image à un seul canal 
 * (par exemple une image en niveaux de gris).
 * 
 * @param src L'image d'entrée à un seul canal (par exemple en niveaux de gris) sur laquelle le flou gaussien sera appliqué.
 * @param dst L'image de sortie dans laquelle l'image floutée sera stockée.
 * @param kernel Le noyau de convolution à appliquer pour le flou gaussien. Le noyau doit être une matrice de type `float` de dimensions impaires.
 * 
 * @return void Cette fonction ne renvoie rien. Le résultat est stocké dans l'image de sortie `dst`.
 * 
 * @note Cette fonction effectue la convolution de l'image `src` avec le noyau spécifié. Elle utilise un format à virgule flottante
 *       pour les calculs intermédiaires afin d'éviter la perte de précision, puis normalise et convertit le résultat final
 *       dans l'échelle des valeurs d'intensité [0, 255].
 * 
 * @warning L'image d'entrée `src` doit être une image à un seul canal, et le noyau `kernel` doit être de taille impair.
 */
void GaussianBlurSingleChannel(const Mat& src, Mat& dst, const Mat& kernel) {
    int kernelRadius = kernel.rows / 2;

    // Créer une image intermédiaire pour le résultat
    Mat temp = Mat::zeros(src.size(), CV_32F); // Utiliser float pour les calculs intermédiaires

    // Effectuer la convolution
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            float sum = 0.0;

            for (int j = -kernelRadius; j <= kernelRadius; j++) {
                for (int i = -kernelRadius; i <= kernelRadius; i++) {
                    int nx = clamp(x + i, 0, src.cols - 1); // Gérer les bords
                    int ny = clamp(y + j, 0, src.rows - 1);
                    sum += src.at<uchar>(ny, nx) * kernel.at<float>(j + kernelRadius, i + kernelRadius);
                }
            }
            temp.at<float>(y, x) = sum; // Stocker le résultat en tant que float
        }
    }

    // Normaliser et reconvertir en format 8 bits
    normalize(temp, temp, 0, 255, NORM_MINMAX); // Normaliser le résultat dans l'intervalle [0, 255]
    temp.convertTo(dst, CV_8UC1); // Convertir en 8 bits unsigned char
}

/**
 * @brief Applique un flou gaussien à une image à plusieurs canaux (couleur ou niveau de gris).
 * 
 * Cette fonction applique un flou gaussien à une image en utilisant un noyau de convolution gaussienne. Elle peut
 * traiter des images en niveaux de gris (1 canal) ou en couleur (3 canaux). Pour une image couleur, le flou gaussien
 * est appliqué indépendamment à chaque canal (R, G, B) de l'image. Si l'image est en niveaux de gris, le flou est appliqué
 * directement sur l'image à un seul canal.
 * 
 * @param src L'image d'entrée (en couleur ou en niveaux de gris) à laquelle le flou gaussien sera appliqué.
 * @param dst L'image de sortie dans laquelle l'image floutée sera stockée.
 * 
 * @return void Cette fonction ne renvoie rien. Le résultat est stocké dans l'image `dst`.
 * 
 * @throws std::runtime_error Si l'image d'entrée `src` n'a pas 1 ou 3 canaux.
 * 
 * @note La fonction applique le flou gaussien à chaque canal de l'image individuellement si l'image est en couleur,
 *       ou directement sur l'image si elle est en niveaux de gris. Un noyau de convolution gaussienne durcodé est utilisé
 *       pour le flou. Après application du filtre, les canaux sont recombinés pour restaurer l'image en couleur (si applicable).
 * 
 * @warning L'image d'entrée doit être une image à 1 ou 3 canaux. Si l'image d'entrée a un nombre de canaux différent, une exception sera levée.
 */
void GaussianBlurMultiChannel(const cv::Mat& src, cv::Mat& dst) {
    // Valider le type d'entrée (image en niveaux de gris ou en couleur)
    if (src.channels() != 3 && src.channels() != 1) {
        throw runtime_error("L'image d'entrée doit être une image à 3 canaux (couleur) ou 1 canal (niveaux de gris).");
    }

    // Obtenir le noyau gaussien durcodé
    Mat kernel = hardcodedGaussianKernel();

    if(src.channels() == 3){
        // Diviser l'image en ses canaux individuels
        vector<cv::Mat> channels;
        split(src, channels);

        // Appliquer le flou gaussien à chaque canal
        for (int i = 0; i < channels.size(); ++i) {
            cv::Mat temp;
            GaussianBlurSingleChannel(channels[i], temp, kernel); // Appliquer le filtre à chaque canal
            channels[i] = temp; // Stocker le résultat dans le vecteur de canaux
        }
        merge(channels, dst);
    }
    if(src.channels() == 1){
        Mat temp;
        GaussianBlurSingleChannel(src, temp, kernel);
        merge(temp, dst);
    }
}

/**
 * @brief Applique un filtre gaussien à une image d'entrée.
 * 
 * Cette fonction applique un filtre gaussien à l'image d'entrée en utilisant la fonction `GaussianBlurMultiChannel`.
 * Le filtre est appliqué pour lisser l'image, réduire le bruit ou produire un effet de flou. La fonction vérifie d'abord
 * que l'image d'entrée n'est pas vide avant d'appliquer le filtre.
 * 
 * @param inputImage L'image d'entrée sur laquelle le filtre gaussien sera appliqué. Cette image peut être en niveaux de gris
 *                   ou en couleur (avec 1 ou 3 canaux).
 * 
 * @return Mat L'image de sortie après application du filtre gaussien.
 * 
 * @throws std::runtime_error Si l'image d'entrée est vide.
 * 
 * @note Cette fonction utilise la méthode `GaussianBlurMultiChannel` pour appliquer le filtre gaussien. Elle supporte à la fois
 *       les images en niveaux de gris et en couleur.
 * 
 * @warning L'image d'entrée ne doit pas être vide.
 */
Mat ImageProccessing::applyGaussianFilter(const Mat& inputImage) {
    // Valider l'image d'entrée
    if (inputImage.empty()) {
        throw runtime_error("L'image d'entrée est vide. Impossible d'appliquer le filtre gaussien.");
    }

    // Initialiser l'image de sortie
    Mat outputImage;

    // Appliquer le flou gaussien en utilisant l'implémentation personnalisée
    GaussianBlurMultiChannel(inputImage, outputImage);

    return outputImage;
}
