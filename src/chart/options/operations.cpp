#include "operations.h"

using namespace Vizzu;
using namespace Vizzu::Diag;

Operations::Operations(OptionsSetterPtr setter,
    const Data::DataTable &table) :
    setter(std::move(setter)),
    table(table)
{}

void Operations::addSeries(const Data::SeriesIndex &index)
{
	const auto &options = setter->getOptions();

	auto subId = options.subAxisType();
	auto mainId = options.mainAxisType();

	const auto &subAxis = options.subAxis();
	const auto &mainAxis = options.mainAxis();

	if (index.getType().isContinous())
	{
		if (subAxis.isPseudoDiscrete())
		{
			setter->addSeries(subId, index);
		}
		else if (mainAxis.isPseudoDiscrete())
		{
			setter->addSeries(mainId, index);
		}
		else if (options.getScales()
						.at(Scale::Size)
						.isPseudoDiscrete())
		{
			if (!Diag::canOverlap(
					(ShapeType::Type)options.shapeType.get()))
				setter->setShape(ShapeType::Circle);
			setter->addSeries(Scale::Size, index);
		}
	}
	else
	{
		if (Diag::canOverlap(
				(ShapeType::Type)options.shapeType.get()))
		{
			setter->addSeries(Scale::Label, index);
		}
		else
		{
			setter->addSeries(subId, index);
		}
	}
}

void Operations::removeSeries(const Data::SeriesIndex &index)
{
	const auto &options = setter->getOptions();

	options.getScales().visitAll(
	    [=](Scale::Type type, const Scale &scale)
	    {
		    if (scale.isSeriesUsed(index))
			    setter->deleteSeries(type, index);
	    });
}

void Operations::split()
{
	const auto &options = setter->getOptions();
	if (options.shapeType.get() == ShapeType::Rectangle
	    && options.getScales().anyAxisSet())
	{
		auto mainId = options.mainAxisType();
		auto subId = options.subAxisType();
		split(mainId, subId);
	}
	if (options.shapeType.get() == ShapeType::Area
	    && options.getScales().anyAxisSet())
	{
		setter->setSplitted(true);
	}
	if (options.shapeType.get() == ShapeType::Circle
	    || options.shapeType.get() == ShapeType::Line
	    || !options.getScales().anyAxisSet())
	{
		split(Scale::Type::Label, Scale::Type::Size);
	}
}

void Operations::split(Diag::Scale::Type mainType, Diag::Scale::Type subType)
{
	const auto &options = setter->getOptions();
	const auto &sub = options.getScales().at(subType);

	if (!sub.discretesIds().empty())
	{
		const auto &series = *sub.discretesIds().begin();

		setter->addSeries(mainType, series);
		setter->deleteSeries(subType, series);
	}
}

void Operations::stack()
{
	const auto &options = setter->getOptions();
	if (options.getScales().anyAxisSet()
	    && options.shapeType.get() == ShapeType::Rectangle)
	{
		auto mainId = options.mainAxisType();
		auto subId = options.subAxisType();
		stack(mainId, subId);
	}
	if (options.shapeType.get() == ShapeType::Area
	    && options.getScales().anyAxisSet())
	{
		setter->setSplitted(false);
	}
	if (options.shapeType.get() == ShapeType::Circle
	    || options.shapeType.get() == ShapeType::Line
	    || !options.getScales().anyAxisSet())
	{
		stack(Scale::Type::Label, Scale::Type::Size);
	}
}

void Operations::stack(Diag::Scale::Type mainType, Diag::Scale::Type subType)
{
	const auto &options = setter->getOptions();
	const auto &main = options.getScales().at(mainType);

	if (!main.discretesIds().empty())
	{
		auto series = *main.discretesIds().rbegin();

		setter->addSeries(subType, series, 0);
		setter->deleteSeries(mainType, series);
	}
}

void Operations::drillDown()
{
	const auto &options = setter->getOptions();
	auto dimensions = options.getScales().getDimensions();

	for (auto series : dimensions)
	{
		auto child =
		    table.getHierarchy().childOf(series.getColIndex());
		if (child)
		{
			auto childId = Data::SeriesIndex(table.getIndex(*child));
			if (!options.getScales().isSeriesUsed(childId))
				addSeries(childId);
		}
	}
}

void Operations::drillUp()
{
	const auto &options = setter->getOptions();
	auto dimensions = options.getScales().getDimensions();

	for (auto series : dimensions)
	{
		auto parent =
		    table.getHierarchy().parentOf(series.getColIndex());
		auto child =
		    table.getHierarchy().childOf(series.getColIndex());
		if (parent)
		{
			auto childUsed =
			    child
			    && options.getScales().isSeriesUsed(
			        Data::SeriesIndex(table.getIndex(*child)));

			auto parentId =
			    Data::SeriesIndex(table.getIndex(*parent));
			if (!childUsed
			    && options.getScales().isSeriesUsed(parentId))
			{
				removeSeries(series);
				return;
			}
		}
	}
}

void Operations::fit(bool /* enable */) {}

void Operations::swapDimension()
{
	const auto &options = setter->getOptions();

	if (!options.getScales().anyAxisSet()) return;

	if (options.shapeType.get() == ShapeType::Rectangle)
	{
		auto subIsCont = !options.subAxis().isPseudoDiscrete();
		auto mainIsCont = !options.mainAxis().isPseudoDiscrete();

		if (subIsCont && mainIsCont)
		{
			const auto &options = setter->getOptions();
			setter->setHorizontal(!(bool)options.horizontal.get());
		}
		else if (subIsCont && !mainIsCont)
		{
			auto cont = *options.subAxis().continousId();
			setter->deleteSeries(options.subAxisType(), cont);
			setter->addSeries(options.mainAxisType(), cont);
		}
		else if (!subIsCont && mainIsCont)
		{
			auto cont = *options.mainAxis().continousId();
			setter->deleteSeries(options.mainAxisType(), cont);
			setter->addSeries(options.subAxisType(), cont);
		}
		else
		{
			const auto &options = setter->getOptions();
			setter->setHorizontal(!(bool)options.horizontal.get());
		}
	}
	else
	{
		const auto &options = setter->getOptions();
		setter->setHorizontal(!(bool)options.horizontal.get());
	}
}

bool Operations::isFit() const
{
	const auto &options = setter->getOptions();

	if (options.shapeType.get() == ShapeType::Rectangle)
	{
		const auto &main = options.mainAxis();
		const auto &sub = options.subAxis();

		if (main.continousId()
		    && (options.alignType.get() == Base::Align::Fit
		        || main.discretesIds().empty() || !sub.continousId()))
		{
			return true;
		}
	}
	return false;
}
