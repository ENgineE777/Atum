#pragma once

class FontCharToGlyphIndexMap
{
public:

	typedef unsigned long CharacterCode;
	typedef signed long GlyphIndex;

	enum
	{
		NumberOfBuckets = 256,
		BucketSize = 256,
		IndexNotFound = -1
	};

	FontCharToGlyphIndexMap()
	{
		this->Indices = 0;
	}

	virtual ~FontCharToGlyphIndexMap()
	{
		if(this->Indices)
		{
			// Free all buckets
			this->clear();

			// Free main structure
			delete [] this->Indices;
			this->Indices = 0;
		}
	}

	void clear()
	{
		if(this->Indices)
		{
			for(int i = 0; i < FontCharToGlyphIndexMap::NumberOfBuckets; i++)
			{
				if(this->Indices[i])
				{
					delete [] this->Indices[i];
					this->Indices[i] = 0;
				}
			}
		}
	}

	const GlyphIndex find(CharacterCode c)
	{
		if(!this->Indices)
		{
			return -1;
		}

		div_t pos = div(c, BucketSize);

		if(!this->Indices[pos.quot])
		{
			return -1;
		}

		const GlyphIndex *ptr = &this->Indices[pos.quot][pos.rem];
		if(*ptr == IndexNotFound)
		{
			return -1;
		}

		return *ptr;
	}

	void insert(CharacterCode c, GlyphIndex g)
	{
		if(!this->Indices)
		{
			this->Indices = new GlyphIndex* [NumberOfBuckets];
			for(int i = 0; i < NumberOfBuckets; i++)
			{
				this->Indices[i] = 0;
			}
		}

		div_t pos = div(c, BucketSize);

		if (!this->Indices[pos.quot])
		{
			this->Indices[pos.quot] = new GlyphIndex [BucketSize];
			for(int i = 0; i < BucketSize; i++)
			{
				this->Indices[pos.quot][i] = IndexNotFound;
			}
		}

		this->Indices[pos.quot][pos.rem] = g;
	}

private:
	GlyphIndex** Indices;
};