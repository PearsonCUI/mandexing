// Mandexing: a manual indexing program for crystallographic data.
// Copyright (C) 2017-2018 Helen Ginn
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 
// Please email: vagabond @ hginn.co.uk for more details.

#include "Tinker.h"
#include <QtCore/qdebug.h>
#include <QtCore/qalgorithms.h>
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qmenubar.h>
#include <QtWidgets/qmessagebox.h>
#include <iostream>
#include <fstream>
#include "RefinementNelderMead.h"
#include "FileReader.h"

#define DEFAULT_WIDTH 1000
#define DEFAULT_HEIGHT 800
#define BUTTON_WIDTH 160
#define BEAM_CENTRE_GROUP_YOFFSET 180
#define BRAVAIS_LATTICE_YOFFSET 580

Tinker::Tinker(QWidget *parent) : QMainWindow(parent)
{
    this->resize(DEFAULT_WIDTH, DEFAULT_HEIGHT);

    this->setWindowTitle("Mandexing");
	QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
	QAction *openAct = fileMenu->addAction(tr("&Open..."));
	connect(openAct, &QAction::triggered, this, &Tinker::openImage);
	QAction *saveAs = fileMenu->addAction(tr("&Save state..."));
	connect(saveAs, &QAction::triggered, this, &Tinker::saveMatrix);
	QAction *loadMatrix = fileMenu->addAction(tr("&Load state..."));
	connect(loadMatrix, &QAction::triggered, this, &Tinker::loadMatrix);
	
	myDialogue = NULL;
	bUnitCell = new QPushButton("Set unit cell", this);
	bUnitCell->setToolTip("Enter crystal's unit cell dimensions");
	bUnitCell->setGeometry(0, 25, BUTTON_WIDTH, 50);
    connect(bUnitCell, SIGNAL(clicked()), this, SLOT(setUnitCellClicked()));

	bAxisOnScreen = new QPushButton("Bring axis on screen", this);
	bAxisOnScreen->setToolTip("Enter two known axes to appear on detector");
	bAxisOnScreen->setGeometry(0, 75, BUTTON_WIDTH, 50);
    connect(bAxisOnScreen, SIGNAL(clicked()), this,
            SLOT(setAxisOnScreenClicked()));

	bFixAxis = new QPushButton("Fix axis", this);
	bFixAxis->setToolTip("Change axis for WASD-key rotation");
	bFixAxis->setGeometry(0, 125, BUTTON_WIDTH, 50);
    connect(bFixAxis, SIGNAL(clicked()), this,
            SLOT(fixAxisClicked()));

	bBeamCentre = new QPushButton("BC", this);
	bBeamCentre->setToolTip("Manually alter beam centre");
	bBeamCentre->setGeometry(55, BEAM_CENTRE_GROUP_YOFFSET + 50, 50, 50);
    connect(bBeamCentre, SIGNAL(clicked()), this,
            SLOT(setBeamCentreClicked()));

	bBeamXMinus = new QPushButton("-", this);
	bBeamXMinus->setGeometry(5, BEAM_CENTRE_GROUP_YOFFSET + 50, 50, 50);
	connect(bBeamXMinus, SIGNAL(clicked()), this,
            SLOT(beamXMinus()));

	bBeamXPlus = new QPushButton("+", this);
	bBeamXPlus->setGeometry(105, BEAM_CENTRE_GROUP_YOFFSET + 50, 50, 50);
	connect(bBeamXPlus, SIGNAL(clicked()), this,
            SLOT(beamXPlus()));

	bBeamYMinus = new QPushButton("-", this);
	bBeamYMinus->setGeometry(55, BEAM_CENTRE_GROUP_YOFFSET + 100, 50, 50);
	connect(bBeamYMinus, SIGNAL(clicked()), this,
            SLOT(beamYMinus()));

	bBeamYPlus = new QPushButton("+", this);
	bBeamYPlus->setGeometry(55, BEAM_CENTRE_GROUP_YOFFSET, 50, 50);
	connect(bBeamYPlus, SIGNAL(clicked()), this,
            SLOT(beamYPlus()));

	bDetDist = new QPushButton("Set detector distance", this);
	bDetDist->setGeometry(0, 330, BUTTON_WIDTH, 50);
    connect(bDetDist, SIGNAL(clicked()), this,
            SLOT(setDetDistClicked()));

	bWavelength = new QPushButton("Set wavelength", this);
	bWavelength->setGeometry(0, 380, BUTTON_WIDTH, 50);
    connect(bWavelength, SIGNAL(clicked()), this,
            SLOT(setWavelengthClicked()));

	bRlpSize = new QPushButton("Set rlp size", this);
	bRlpSize->setToolTip("Change size of reciprocal lattice points");
	bRlpSize->setGeometry(0, 430, BUTTON_WIDTH, 50);
    connect(bRlpSize, SIGNAL(clicked()), this,
            SLOT(setRlpSizeClicked()));

	bDegrees = new QPushButton("Set degree step", this);
	bDegrees->setToolTip("Change increment of rotations by WASD keys");
	bDegrees->setGeometry(0, 480, BUTTON_WIDTH, 50);
    connect(bDegrees, SIGNAL(clicked()), this,
            SLOT(setDegreeStepClicked()));

	bRefine = new QPushButton("Refine", this);
	bRefine->setToolTip("Click once; choose strong reflections; click again");
	bRefine->setGeometry(0, 530, BUTTON_WIDTH, 50);
    connect(bRefine, SIGNAL(clicked()), this,
            SLOT(refineClicked()));

	bLatPrimitive = new QPushButton("P", this);
	bLatPrimitive->setToolTip("Choose primitive lattice");
	bLatPrimitive->setGeometry(30, BRAVAIS_LATTICE_YOFFSET, 50, 50);
	connect(bLatPrimitive, &QPushButton::clicked,
			[=]{ changeLattice(BravaisLatticePrimitive); });

	bLatBase = new QPushButton("C", this);
	bLatBase->setToolTip("Choose base-centred lattice");
	bLatBase->setGeometry(80, BRAVAIS_LATTICE_YOFFSET, 50, 50);
	connect(bLatBase, &QPushButton::clicked,
			[=]{ changeLattice(BravaisLatticeBase); });

	bLatBody = new QPushButton("I", this);
	bLatBody->setToolTip("Choose body-centred lattice");
	bLatBody->setGeometry(30, BRAVAIS_LATTICE_YOFFSET + 50, 50, 50);
	connect(bLatBody, &QPushButton::clicked,
			[=]{ changeLattice(BravaisLatticeBody); });

	bLatFace = new QPushButton("F", this);
	bLatFace->setToolTip("Choose face-centred lattice");
	bLatFace->setGeometry(80, BRAVAIS_LATTICE_YOFFSET + 50, 50, 50);
	connect(bLatFace, &QPushButton::clicked,
			[=]{ changeLattice(BravaisLatticeFace); });


    bResolution = new QPushButton("Set resolution", this);
	bResolution->setToolTip("Set maximum calculated resolution");
	bResolution->setGeometry(0, 680, BUTTON_WIDTH, 50);
	connect(bResolution, SIGNAL(clicked()), this,
			SLOT(setResolutionClicked()));

    bIdentifyHkl = new QPushButton("Identify hkl", this);
	bResolution->setToolTip("Click once; choose a few reflections; click off");
	bIdentifyHkl->setGeometry(0, 730, BUTTON_WIDTH, 50);
	connect(bIdentifyHkl, SIGNAL(clicked()), this,
			SLOT(identifyHkl()));


	_refineStage = 0;
	_fixAxisStage = 0;
    _identifyHklStage = 0;
    
	fileDialogue = NULL;

    imageLabel = new QLabel(this);
    imageLabel->setScaledContents(true);
    imageLabel->setGeometry(0, 0, DEFAULT_HEIGHT, DEFAULT_HEIGHT);
    imageLabel->show();
	bUnitCell->show();
	
	QBrush brush(Qt::transparent);
	
	_crystal.setTinker(this);

	overlayView = new PredictionView(imageLabel);
	overlay = new QGraphicsScene(overlayView);
	overlayView->setCrystal(&_crystal);
	overlayView->setDetector(&_detector);
	overlayView->setTinker(this);
	overlayView->setBackgroundBrush(brush);
	overlayView->setGeometry(0, 0, DEFAULT_HEIGHT, DEFAULT_HEIGHT);
	overlayView->setStyleSheet("background-color: transparent;");
	overlayView->setScene(overlay);

	overlayView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	overlayView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	overlayView->show();

	_detector.setCrystal(&_crystal);
    
    _notice = new QLabel("Load an image (.png, .jpg, etc.)\n"\
                         "from the file menu.", this);
    _notice->setGeometry((DEFAULT_WIDTH / 2 + BUTTON_WIDTH - 200), 
                         DEFAULT_HEIGHT / 2 - 20,
                         200, 40);
    _notice->show();
}

void Tinker::resizeEvent(QResizeEvent *)
{
    int w = width();
    int h = height();
    int max_w = w;
    int max_h = h;
    
    h = (h < w) ? h : w;
    w = (w < h) ? w : h;
    
    int top = max_h / 2 - h / 2;
    int left = max_w / 2 - w / 2;
    
    if (left < BUTTON_WIDTH) left = BUTTON_WIDTH + 10;

    imageLabel->setGeometry(left, top, w, h);
	overlayView->setGeometry(0, 0, w, h);
	drawPredictions();
}

void Tinker::setup()
{

}

void Tinker::setUnitCellClicked()
{
	delete myDialogue;
    myDialogue = new Dialogue(this, "Choose unit cell (Å, º)",
                              "Enter a b c alpha beta gamma:",
                              "79.2 79.2 38.0 90 90 90",
                              "Set unit cell");
	myDialogue->setTag(DialogueUnitCell);
    myDialogue->setTinker(this);
	myDialogue->show();
}

void Tinker::setAxisOnScreenClicked()
{
	delete myDialogue;
    myDialogue = new Dialogue(this, "Bring axis on screen",
                              "Enter one or two Miller index vectors:",
                              "1 0 0 0 0 1",
                              "Set axis");
	myDialogue->setTag(DialogueBringAxis);
    myDialogue->setTinker(this);
	myDialogue->show();
}

void Tinker::setBeamCentreClicked()
{
	delete myDialogue;
    myDialogue = new Dialogue(this, "Set beam centre (pixels)",
                              "Enter new beam centre:",
                              "2200 2200",
                              "Set centre");
	myDialogue->setTag(DialogueBeamCentre);
    myDialogue->setTinker(this);
	myDialogue->show();
}


void Tinker::setRlpSizeClicked()
{
	delete myDialogue;
    myDialogue = new Dialogue(this, "Set rlp size (Å^-1)",
    								"Enter new rlp size:",
    								"0.002",
    								"Set rlp size");
	myDialogue->setTag(DialogueRlpSize);
    myDialogue->setTinker(this);
	myDialogue->show();
}

void Tinker::beamXPlus()
{
	changeBeamCentre(1, 0);
}

void Tinker::beamYPlus()
{
	changeBeamCentre(0, -1);

}

void Tinker::beamXMinus()
{
	changeBeamCentre(-1, 0);
}

void Tinker::beamYMinus()
{
	changeBeamCentre(0, 1);
}

void Tinker::setResolutionClicked()
{
	myDialogue = new Dialogue(this, "Set resolution (Å)",
    								"Enter new resolution:",
    								"1.8",
    								"Set resolution");
	myDialogue->setTag(DialogueResolution);
    myDialogue->setTinker(this);
	myDialogue->show();
}

void Tinker::setDetDistClicked()
{
	myDialogue = new Dialogue(this, "Set distance (pixels)",
    								"Enter new detector distance:",
    								"174286",
    								"Set distance");
	myDialogue->setTag(DialogueDistance);
    myDialogue->setTinker(this);
	myDialogue->show();
}

void Tinker::setWavelengthClicked()
{
	myDialogue = new Dialogue(this, "Set wavelength (Å)",
    								"Enter new wavelength:",
    								"0.0251",
    								"Set wavelength");
	myDialogue->setTag(DialogueWavelength);
    myDialogue->setTinker(this);
	myDialogue->show();
}

void Tinker::setDegreeStepClicked()
{
	myDialogue = new Dialogue(this, "Set degree step (º)",
    								"Enter degrees per key press:",
    								"0.12",
    								"Set degrees");
	myDialogue->setTag(DialogueDegreeStep);
    myDialogue->setTinker(this);
	myDialogue->show();
}

void Tinker::changeBeamCentre(double deltaX, double deltaY)
{
	_detector.adjustBeamCentre(deltaX, deltaY);
	drawPredictions();
}

void Tinker::transformToDetectorCoordinates(int *x, int *y)
{
	double w = blankImage.width();
	double h = blankImage.height();
	double w2 = overlayView->width();
	double h2 = overlayView->height();
	double bx = _detector.getBeamCentre().x;
	double by = _detector.getBeamCentre().y;
	double window_bx = bx * w2 / w;
	double window_by = by * h2 / h;
	
	std::cout << *x << ", " << *y << " to ";
	
	*x = (*x - window_bx) * w / w2 + bx;
	*y = (*y - window_by) * w / w2 + by;

	std::cout << *x << ", " << *y << std::endl;
}

void Tinker::drawPredictions()
{
	_detector.calculatePositions();
	qDeleteAll(overlay->items());
	overlay->clear();
	
	double w = blankImage.width();
	double h = blankImage.height();
	double w2 = overlayView->width();
	double h2 = overlayView->height();
	double bx = _detector.getBeamCentre().x;
	double by = _detector.getBeamCentre().y;
	
	overlay->setSceneRect(overlayView->geometry());
	
	bx *= w2 / w;
	by *= h2 / h;
	
	int ellipseSize = 10;
	
	for (size_t i = 0; i < _crystal.millerCount(); i++)
	{
		bool onImage = _crystal.shouldDisplayMiller(i);

		if (!onImage)
		{
			continue;
		}
		
		double weight = _crystal.weightForMiller(i);
		if (weight < 0) continue;
		
		vec3 pos = _crystal.position(i);
		
		QPen pen = QPen(QColor(0, 0, 0, (1 - weight) * 255));
		bool watching = _crystal.isBeingWatched(i);
		
		if (_refineStage != 1) watching = false;
		
		QBrush brush(Qt::transparent);

		if (watching)
		{
			brush = QBrush(QColor(255, 255, 255, 50));
		}
		
		pos.x = w2 * pos.x / w + bx;
		pos.y = h2 * pos.y / h + by;
		
		if (pos.x < 20 || pos.y < 20 || pos.x > w2 - 20 || pos.y > h2 - 20)
		{
			continue;
		}

		overlay->addEllipse(pos.x - ellipseSize / 2, pos.y - ellipseSize / 2,
		 					ellipseSize, ellipseSize, pen, brush);	
	}
	
	/* Draw basis vectors for crystal in real space */
	
	mat3x3 scaled_basis = _crystal.getScaledBasisVectors();
	
	for (size_t i = 0; i < 3; i++)
	{
		QPen pen = QPen(QColor(255, 0, 0));
		vec3 basis_vector = mat3x3_axis(scaled_basis, i);
		basis_vector.x += bx;
		basis_vector.y += by;
		
		overlay->addLine(bx, by, basis_vector.x, basis_vector.y, pen);
	}
	
	/* Draw fixed axis, if exists */
	vec3 axis = _crystal.getFixedAxis();
	
	if (vec3_length(axis) > 0.5) // is set
	{
		QPen purple = QPen(QColor(255, 64, 255));
		vec3_mult(&axis, 100);
		overlay->addLine(-axis.x + bx, -axis.y + bx,
						  axis.x + bx, axis.y + bx, purple);
	}
}

void Tinker::receiveDialogue(DialogueType type, std::string diagString)
{
	std::cout << "String: (" << (diagString) << ")" << std::endl;
	std::vector<double> trial;

	if (!diagString.length())
	{
		goto cleanup_dialogue;
	}
	
	while (true)
	{
		size_t pos = diagString.find(" ");
		
		if (pos >= diagString.length()) pos = diagString.length();
		
		std::string substr = diagString.substr(0, pos);
		double value = atof(substr.c_str());
		trial.push_back(value);
		
		std::cout << substr << " ";		
		if (pos >= diagString.length()) break;
		diagString = diagString.substr(pos + 1, diagString.length() - pos - 1);
	}
	
	if (type == DialogueUnitCell)
	{
		std::cout << std::endl;
		std::cout << "Unit cell has " << trial.size() << " parameters." << std::endl;
	
		if (trial.size() != 6)
		{
			goto cleanup_dialogue;
		}
		else
		{
			_crystal.setUnitCell(trial);
			drawPredictions();
		}
	}
	else if (type == DialogueBringAxis)
	{
		std::cout << "Axis has " << trial.size() << " parameters." << std::endl;
		
		if (trial.size() < 3)
		{
			goto cleanup_dialogue;
		}
		else
		{
			_crystal.bringAxisToScreen(trial);
			drawPredictions();
		}
	}
	else if (type == DialogueBeamCentre)
	{
		std::cout << "Beam centre has " << trial.size() << " parameters." << std::endl;
		
		if (trial.size() != 2)
		{
			goto cleanup_dialogue;
		}
		else
		{
			_detector.setBeamCentre(trial[0], trial[1]);
			drawPredictions();
		}
	}
	else if (type == DialogueResolution)
	{
		std::cout << "Resolution has " << trial.size() << " parameters." << std::endl;
		
		if (trial.size() != 1)
		{
			goto cleanup_dialogue;
		}
		else
		{
			_crystal.setResolution(trial[0]);
			_crystal.populateMillers();
			drawPredictions();
		}
	}
	else if (type == DialogueDistance)
	{
		std::cout << "Distance has " << trial.size() << " parameters." << std::endl;
		
		if (trial.size() != 1)
		{
			goto cleanup_dialogue;
		}
		else
		{
			_detector.setDetectorDistance(trial[0]);
			drawPredictions();
		}
	}
	else if (type == DialogueWavelength)
	{
		std::cout << "Wavelength has " << trial.size() << " parameters." << std::endl;
		
		if (trial.size() != 1)
		{
			goto cleanup_dialogue;
		}
		else
		{
			_detector.setWavelength(trial[0]);
			_crystal.setWavelength(trial[0]);
			drawPredictions();
		}
	}
	else if (type == DialogueRlpSize)
	{
		std::cout << "Rlp size has " << trial.size() << " parameters." << std::endl;
		
		if (trial.size() != 1)
		{
			goto cleanup_dialogue;
		}
		else
		{
			_crystal.setRlpSize(trial[0]);
			_crystal.populateMillers();
			drawPredictions();
		}
	}
	else if (type == DialogueDegreeStep)
	{
		std::cout << "Degree size has " << trial.size() << " parameters." << std::endl;
		
		if (trial.size() != 1)
		{
			goto cleanup_dialogue;
		}
		else
		{
			double radians = deg2rad(trial[0]);
			overlayView->setRadiansPerKeyPress(radians);
			drawPredictions();
		}
	}

cleanup_dialogue:
	myDialogue->cleanup();
	myDialogue->hide();
	myDialogue->disconnect();
	myDialogue->deleteLater();

	myDialogue = NULL;
}

void Tinker::openImage()
{
	delete fileDialogue;
	fileDialogue = new QFileDialog(this, tr("Open images"),
									 tr("Image Files (*.png *.jpg *.tif *.bmp)"));
	fileDialogue->setFileMode(QFileDialog::AnyFile);
	fileDialogue->show();
	
	QStringList fileNames;
	if (fileDialogue->exec())
	{
    	fileNames = fileDialogue->selectedFiles();
    }
    
    std::cout << "Read " << fileNames.size() << std::endl;
    
	if (fileNames.size() >= 1)
	{
		if (!blankImage.load(fileNames[0]))
		{
			qDebug("Error loading image");
			return;
		}

		bool first = false;

		if (!imageLabel->pixmap())
		{
			first = true;
		}

		std::string filename = fileNames[0].toStdString();
		std::string newTitle = "Mandexing - " + getFilename(filename);
		
		this->setWindowTitle(newTitle.c_str());

		_notice->hide();
		imageLabel->setPixmap(blankImage);
		if (first)
		{
			_detector.setBeamCentre(blankImage.width() / 2,
		   	                        blankImage.height() / 2);
		}
	}
}

void Tinker::loadMatrix()
{
	delete fileDialogue;
	fileDialogue = new QFileDialog(this, tr("Load state"),
									 tr("Mandexing matrix file (*.dat)"));
	fileDialogue->setFileMode(QFileDialog::AnyFile);
	fileDialogue->show();
	
	QStringList fileNames;
	if (fileDialogue->exec())
	{
		fileNames = fileDialogue->selectedFiles();
	}
    
    if (fileNames.size() >= 1)
	{
		std::string filename = fileNames[0].toStdString();
		std::string matrix = get_file_contents(filename);
		
		std::vector<std::string> lines = split(matrix, '\n');
		
		QMessageBox *msgBox = new QMessageBox(this);
		msgBox->setStandardButtons(QMessageBox::Ok);
		msgBox->setDefaultButton(QMessageBox::Ok);
		msgBox->setWindowModality(Qt::NonModal);
		msgBox->setText("Sorry no");
		
		for (size_t i = 0; i < lines.size(); i++)
		{
			std::vector<std::string> components = split(lines[i], ' ');
			
			if (components.size() == 0)
			{
				continue;
			}

			if (components[0] == "rotation" || components[0] == "unitcell")
			{
				if (components.size() < 10)
				{
					msgBox->setInformativeText("Not enough components,"\
					                           "expecting 9 space-separated "\
					                           "values. Try again.");
					msgBox->exec();
				}
			}

			if (components[0] == "rotation")
			{
				mat3x3 rotMat = mat3x3_from_string(components);
				_crystal.setRotation(rotMat);
			}

			if (components[0] == "unitcell")
			{
				mat3x3 unitCell = mat3x3_from_string(components);
				_crystal.setUnitCell(unitCell);
			}

			if (components[0] == "det_centre")
			{
				if (components.size() < 4)
				{
					msgBox->setInformativeText("Not enough components,"\
					                           "expecting 3 space-separated "\
					                           "values. Try again.");
					msgBox->exec();
				}

				vec3 centre = vec3_from_string(components);
				_detector.setBeamCentre(centre.x, centre.y);
				_detector.setDetectorDistance(centre.z);
			}
			
			if (components[0] == "wavelength")
			{
				if (components.size() < 2)
				{
					msgBox->setInformativeText("Not enough components,"\
					                           "expecting 1 "\
					                           "value. Try again.");
					msgBox->exec();
				}

				double wave = atof(components[1].c_str());
				_detector.setWavelength(wave);
				_crystal.setWavelength(wave);
			}
			
			if (components[0] == "rlp_size")
			{
				if (components.size() < 2)
				{
					msgBox->setInformativeText("Not enough components,"\
					                           "expecting 1 "\
					                           "value. Try again.");
					msgBox->exec();
				}

				double rlp_size = atof(components[1].c_str());
				_crystal.setRlpSize(rlp_size);
			}
		}

		_crystal.populateMillers();
		drawPredictions();
		
		delete msgBox;
	}
}
    
void Tinker::saveMatrix()
{
	delete fileDialogue;
	fileDialogue = new QFileDialog(this, tr("Save as .dat file"),
	                               tr("matrix.dat"));
	fileDialogue->setFileMode(QFileDialog::AnyFile);
	fileDialogue->setAcceptMode(QFileDialog::AcceptSave);
	fileDialogue->show();
	
	QStringList fileNames;
	if (fileDialogue->exec())
	{
    	fileNames = fileDialogue->selectedFiles();
    }
    
    if (fileNames.size() >= 1)
	{
		std::ofstream file;
		file.open(fileNames[0].toStdString().c_str());
		
		mat3x3 rot = _crystal.getRotation();
		mat3x3 unitCell = _crystal.getUnitCell();
		vec3 beamCentre = _detector.getBeamCentre();
		
		file << "rotation ";
		file << computer_friendly_desc(rot);
		
		file << "unitcell ";
		file << computer_friendly_desc(unitCell);
		
		file << "det_centre ";
		file << computer_friendly_desc(beamCentre);
		
		file << "wavelength ";
		file << _detector.getWavelength() << std::endl;

		file << "rlpsize ";
		file << _crystal.getRlpSize() << std::endl;

		file.close();
	}
}

void Tinker::fixAxisClicked()
{
	if (_fixAxisStage == 0)
	{
		_fixAxisStage = 1;
		bFixAxis->setText("Choose two points");
		overlayView->setFixAxisStage(_fixAxisStage);
	}
	else
	{
		bFixAxis->setText("Fix axis");
		overlayView->setFixAxisStage(-1);
		_fixAxisStage = 0;		
	}
}

void Tinker::finishFixAxis()
{
	_fixAxisStage = 0;
	bFixAxis->setText("Fix axis");	
	drawPredictions();
}

void Tinker::changeLattice(BravaisLatticeType type)
{
	_crystal.setBravaisLattice(type);
	_crystal.populateMillers();
	drawPredictions();
}

void Tinker::startRefinement()
{
	_refineStage = 2;
	bRefine->setText("Refining...");
	
	NelderMeadPtr mead = NelderMeadPtr(new NelderMead());
	mead->setEvaluationFunction(Crystal::ewaldSphereClosenessScore, &_crystal);
	mead->addParameter(&_crystal, Crystal::getHorizontal, Crystal::setHorizontal, 0.002, 0.0002);
	mead->addParameter(&_crystal, Crystal::getVertical, Crystal::setVertical, 0.002, 0.0002);
	mead->setCycles(15);
	mead->refine();
	
	_crystal.clearUpRefinement();
	_refineStage = 0;
	bRefine->setText("Refine");
	
//	QtConcurrent::run(RefinementStrategy::run, &*mead);
}

void Tinker::refineClicked()
{
	if (_refineStage == 0)
	{
		_detector.prepareLookupTable();
		_refineStage = 1;
		bRefine->setText("Choose good spots");
		overlayView->setRefineStage(_refineStage);
	}
	else
	{
		bRefine->setText("Refine");
		overlayView->setRefineStage(0);
		_refineStage = 0;		
	}
}


void Tinker::identifyHkl()
{
    if (_identifyHklStage == 0)
    {
		_detector.prepareLookupTable();
        _identifyHklStage = 1;
        //Button name.
        bIdentifyHkl->setText("Choose refl");
        overlayView->setIdentifyHklStage(_identifyHklStage);
    }
    else
    {
        bIdentifyHkl->setText("Identify hkl");
        overlayView->setIdentifyHklStage(0);
        _identifyHklStage = 0;
    }
}

Tinker::~Tinker()
{
	delete bUnitCell;
	delete imageLabel;
}
